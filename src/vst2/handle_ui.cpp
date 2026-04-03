// This is the most infuriating thing ever

#include "SDL.h"
#include "SDL_events.h"
#include "SDL_scancode.h"
#include "SDL_syswm.h"
#include "src/divizion.hpp"
#include "src/gui.hpp"
#include "src/impl_actions.hpp"
#include "imgui_internal.h"
#include <windef.h>
#include <windows.h>
#include <commctrl.h>

struct SubclassData {
  SDL_Window* sdlWindow;
  bool hasFocus{ false }; // track current state
};

static std::unordered_map<Divizion*, Gui*> effEditInstances = {};
static bool globalGuiInit = false;

static ImGuiKey ImGui_ImplSDL2_KeycodeToImGuiKey(int keycode);

void handleOpen(Divizion* di, void* ptr)
{
  if (!di) return;

  if (!globalGuiInit) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      logE("can't init SDL video %s", SDL_GetError());
      return;
    }
    globalGuiInit = true;
  }
  HWND parent = (HWND)ptr;
  HWND cwindow = CreateWindowExA(
    0, "STATIC", "",
    WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | SS_NOTIFY, 0, 0,
    500, 500, parent, nullptr, nullptr, nullptr);
  if (!cwindow) {
    logE("can't init own window");
    return;
  }
  SDL_Window* win = SDL_CreateWindowFrom(cwindow);
  if (!win) {
    logE("can't init SDL window from own window");
    return;
  }
  SetFocus(cwindow);
  DivizionActionsImpl* i = new DivizionActionsImpl(di->e);
  Gui* g = new Gui(win, i);
  effEditInstances[di] = g;
}

void handleIdle(Divizion* di)
{
  if (!di) return;
  if (!effEditInstances.count(di)) return;
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    uint32_t wid = 0;
    if (e.type == SDL_WINDOWEVENT) wid = e.window.windowID;
    else if (e.type >= SDL_KEYDOWN && e.type <= SDL_KEYUP) wid = e.key.windowID;
    else if (e.type >= SDL_MOUSEMOTION && e.type <= SDL_MOUSEWHEEL)
      wid = e.button.windowID;
    else if (e.type == SDL_TEXTINPUT) wid = e.text.windowID;

    if (Gui::windows.count(wid)) {
      Gui* active = Gui::windows[wid];
      ImGui::SetCurrentContext(active->c);
      ImGuiIO& io = ImGui::GetIO();
      switch (e.type) {
      case SDL_WINDOWEVENT:
        switch (e.window.event) {
        case SDL_WINDOWEVENT_FOCUS_GAINED:
          io.AddFocusEvent(true);
          break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
          io.AddFocusEvent(false);
          break;
        }
        break;
      case SDL_MOUSEMOTION:
        io.AddMousePosEvent((float)e.motion.x, (float)e.motion.y);
        break;
      case SDL_MOUSEBUTTONDOWN: {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(active->w, &wmInfo);
        HWND hwnd = wmInfo.info.win.window;
        if (GetFocus() != hwnd) // only call if not already focused
          SetFocus(hwnd);
        io.AddMouseButtonEvent(e.button.button - 1, true);
        break;
      }
      case SDL_MOUSEBUTTONUP: {
        io.AddMouseButtonEvent(e.button.button - 1, false);
        break;
      }
      case SDL_MOUSEWHEEL:
        io.AddMouseWheelEvent(e.wheel.x, e.wheel.y);
        break;
      case SDL_KEYDOWN:
        logI("SDL_KEYDOWN: sym=%d scancode=%d repeat=%d", (int)e.key.keysym.sym,
             (int)e.key.keysym.scancode, (int)e.key.repeat);
      case SDL_KEYUP: {
        logI("key down");
        // Handle Modifiers (Ctrl, Shift, Alt, Gui)
        io.AddKeyEvent(ImGuiMod_Ctrl, (e.key.keysym.mod & KMOD_CTRL) != 0);
        io.AddKeyEvent(ImGuiMod_Shift, (e.key.keysym.mod & KMOD_SHIFT) != 0);
        io.AddKeyEvent(ImGuiMod_Alt, (e.key.keysym.mod & KMOD_ALT) != 0);
        io.AddKeyEvent(ImGuiMod_Super, (e.key.keysym.mod & KMOD_GUI) != 0);
        // Map SDL Keycode to ImGuiKey
        ImGuiKey key = ImGui_ImplSDL2_KeycodeToImGuiKey(e.key.keysym.sym);
        io.AddKeyEvent(key, (e.type == SDL_KEYDOWN));
        logI("ImGui key=%d isDown=%d wantCapture=%d activeId=%d", (int)key,
             (int)e.type == SDL_KEYDOWN, (int)io.WantCaptureKeyboard,
             (int)ImGui::GetCurrentContext()->ActiveId);
        io.SetKeyEventNativeData(key, e.key.keysym.sym, e.key.keysym.scancode,
                                 e.key.keysym.scancode);
        break;
      }
      case SDL_TEXTINPUT:
        io.AddInputCharactersUTF8(e.text.text);
        break;
      }
    }
  }
  effEditInstances[di]->RenderGui();
}

void handleClose(Divizion* di)
{
  if (!di) return;
  if (!effEditInstances.count(di)) return;

  SDL_Window* w = effEditInstances[di]->w;
  delete effEditInstances[di];
  effEditInstances.erase(di);
  SDL_DestroyWindow(w);
}

ImGuiKey ImGui_ImplSDL2_KeycodeToImGuiKey(int keycode)
{
  logI("kc %d", keycode);
  switch (keycode) {
  case SDLK_TAB:
    return ImGuiKey_Tab;
  case SDLK_LEFT:
    return ImGuiKey_LeftArrow;
  case SDLK_RIGHT:
    return ImGuiKey_RightArrow;
  case SDLK_UP:
    return ImGuiKey_UpArrow;
  case SDLK_DOWN:
    return ImGuiKey_DownArrow;
  case SDLK_PAGEUP:
    return ImGuiKey_PageUp;
  case SDLK_PAGEDOWN:
    return ImGuiKey_PageDown;
  case SDLK_HOME:
    return ImGuiKey_Home;
  case SDLK_END:
    return ImGuiKey_End;
  case SDLK_INSERT:
    return ImGuiKey_Insert;
  case SDLK_DELETE:
    return ImGuiKey_Delete;
  case SDLK_BACKSPACE:
    return ImGuiKey_Backspace;
  case SDLK_SPACE:
    return ImGuiKey_Space;
  case SDLK_RETURN:
    return ImGuiKey_Enter;
  case SDLK_ESCAPE:
    return ImGuiKey_Escape;
  case SDLK_QUOTE:
    return ImGuiKey_Apostrophe;
  case SDLK_COMMA:
    return ImGuiKey_Comma;
  case SDLK_MINUS:
    return ImGuiKey_Minus;
  case SDLK_PERIOD:
    return ImGuiKey_Period;
  case SDLK_SLASH:
    return ImGuiKey_Slash;
  case SDLK_SEMICOLON:
    return ImGuiKey_Semicolon;
  case SDLK_EQUALS:
    return ImGuiKey_Equal;
  case SDLK_LEFTBRACKET:
    return ImGuiKey_LeftBracket;
  case SDLK_BACKSLASH:
    return ImGuiKey_Backslash;
  case SDLK_RIGHTBRACKET:
    return ImGuiKey_RightBracket;
  case SDLK_BACKQUOTE:
    return ImGuiKey_GraveAccent;
  case SDLK_CAPSLOCK:
    return ImGuiKey_CapsLock;
  case SDLK_SCROLLLOCK:
    return ImGuiKey_ScrollLock;
  case SDLK_NUMLOCKCLEAR:
    return ImGuiKey_NumLock;
  case SDLK_PRINTSCREEN:
    return ImGuiKey_PrintScreen;
  case SDLK_PAUSE:
    return ImGuiKey_Pause;
  case SDLK_KP_0:
    return ImGuiKey_Keypad0;
  case SDLK_KP_1:
    return ImGuiKey_Keypad1;
  case SDLK_KP_2:
    return ImGuiKey_Keypad2;
  case SDLK_KP_3:
    return ImGuiKey_Keypad3;
  case SDLK_KP_4:
    return ImGuiKey_Keypad4;
  case SDLK_KP_5:
    return ImGuiKey_Keypad5;
  case SDLK_KP_6:
    return ImGuiKey_Keypad6;
  case SDLK_KP_7:
    return ImGuiKey_Keypad7;
  case SDLK_KP_8:
    return ImGuiKey_Keypad8;
  case SDLK_KP_9:
    return ImGuiKey_Keypad9;
  case SDLK_KP_PERIOD:
    return ImGuiKey_KeypadDecimal;
  case SDLK_KP_DIVIDE:
    return ImGuiKey_KeypadDivide;
  case SDLK_KP_MULTIPLY:
    return ImGuiKey_KeypadMultiply;
  case SDLK_KP_MINUS:
    return ImGuiKey_KeypadSubtract;
  case SDLK_KP_PLUS:
    return ImGuiKey_KeypadAdd;
  case SDLK_KP_ENTER:
    return ImGuiKey_KeypadEnter;
  case SDLK_KP_EQUALS:
    return ImGuiKey_KeypadEqual;
  case SDLK_LCTRL:
    return ImGuiKey_LeftCtrl;
  case SDLK_LSHIFT:
    return ImGuiKey_LeftShift;
  case SDLK_LALT:
    return ImGuiKey_LeftAlt;
  case SDLK_LGUI:
    return ImGuiKey_LeftSuper;
  case SDLK_RCTRL:
    return ImGuiKey_RightCtrl;
  case SDLK_RSHIFT:
    return ImGuiKey_RightShift;
  case SDLK_RALT:
    return ImGuiKey_RightAlt;
  case SDLK_RGUI:
    return ImGuiKey_RightSuper;
  case SDLK_APPLICATION:
    return ImGuiKey_Menu;
  case SDLK_0:
    return ImGuiKey_0;
  case SDLK_1:
    return ImGuiKey_1;
  case SDLK_2:
    return ImGuiKey_2;
  case SDLK_3:
    return ImGuiKey_3;
  case SDLK_4:
    return ImGuiKey_4;
  case SDLK_5:
    return ImGuiKey_5;
  case SDLK_6:
    return ImGuiKey_6;
  case SDLK_7:
    return ImGuiKey_7;
  case SDLK_8:
    return ImGuiKey_8;
  case SDLK_9:
    return ImGuiKey_9;
  case SDLK_a:
    return ImGuiKey_A;
  case SDLK_b:
    return ImGuiKey_B;
  case SDLK_c:
    return ImGuiKey_C;
  case SDLK_d:
    return ImGuiKey_D;
  case SDLK_e:
    return ImGuiKey_E;
  case SDLK_f:
    return ImGuiKey_F;
  case SDLK_g:
    return ImGuiKey_G;
  case SDLK_h:
    return ImGuiKey_H;
  case SDLK_i:
    return ImGuiKey_I;
  case SDLK_j:
    return ImGuiKey_J;
  case SDLK_k:
    return ImGuiKey_K;
  case SDLK_l:
    return ImGuiKey_L;
  case SDLK_m:
    return ImGuiKey_M;
  case SDLK_n:
    return ImGuiKey_N;
  case SDLK_o:
    return ImGuiKey_O;
  case SDLK_p:
    return ImGuiKey_P;
  case SDLK_q:
    return ImGuiKey_Q;
  case SDLK_r:
    return ImGuiKey_R;
  case SDLK_s:
    return ImGuiKey_S;
  case SDLK_t:
    return ImGuiKey_T;
  case SDLK_u:
    return ImGuiKey_U;
  case SDLK_v:
    return ImGuiKey_V;
  case SDLK_w:
    return ImGuiKey_W;
  case SDLK_x:
    return ImGuiKey_X;
  case SDLK_y:
    return ImGuiKey_Y;
  case SDLK_z:
    return ImGuiKey_Z;
  case SDLK_F1:
    return ImGuiKey_F1;
  case SDLK_F2:
    return ImGuiKey_F2;
  case SDLK_F3:
    return ImGuiKey_F3;
  case SDLK_F4:
    return ImGuiKey_F4;
  case SDLK_F5:
    return ImGuiKey_F5;
  case SDLK_F6:
    return ImGuiKey_F6;
  case SDLK_F7:
    return ImGuiKey_F7;
  case SDLK_F8:
    return ImGuiKey_F8;
  case SDLK_F9:
    return ImGuiKey_F9;
  case SDLK_F10:
    return ImGuiKey_F10;
  case SDLK_F11:
    return ImGuiKey_F11;
  case SDLK_F12:
    return ImGuiKey_F12;
  }
  return ImGuiKey_None;
}
