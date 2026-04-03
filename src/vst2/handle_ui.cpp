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
static SDL_Scancode VKToSDLScancode(WPARAM vk);
static LRESULT CALLBACK PluginSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                           LPARAM lParam, UINT_PTR uIdSubclass,
                                           DWORD_PTR dwRefData);
static bool IsAncestor(HWND ancestor, HWND child);

#define WM_RECLAIM_FOCUS (WM_APP + 1)

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
  SubclassData* scData = new SubclassData{ win };
  SetFocus(cwindow);
  SetWindowSubclass(cwindow, PluginSubclassProc, 1, (DWORD_PTR)scData);
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
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  if (SDL_GetWindowWMInfo(w, &wmInfo)) {
    HWND hwnd = wmInfo.info.win.window;
    DWORD_PTR scData = 0;
    GetWindowSubclass(hwnd, PluginSubclassProc, 1, &scData);
    RemoveWindowSubclass(hwnd, PluginSubclassProc, 1);
    delete reinterpret_cast<SubclassData*>(scData);
  }
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

// this bullshit for solving the key focus because VST windows are weird
LRESULT CALLBACK PluginSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam, UINT_PTR uIdSubclass,
                                    DWORD_PTR dwRefData)
{
  SubclassData* data = reinterpret_cast<SubclassData*>(dwRefData);

  switch (msg) {
  case WM_SETFOCUS:
    logI("WM_SETFOCUS: gaining=%p hasFocus=%d", (void*)wParam, data->hasFocus);
    break;
  case WM_KILLFOCUS: {
    HWND gaining = (HWND)wParam;
    logI("WM_KILLFOCUS: gaining=%p isAncestor=%d hasFocus=%d", (void*)gaining,
         gaining ? IsAncestor(gaining, hwnd) : -1, data->hasFocus);
    break;
  }
  case WM_KEYDOWN:
  case WM_SYSKEYDOWN:
    logI("WM_KEYDOWN: vk=%llu repeat=%d", wParam, (lParam >> 30) & 1);
    break;
  }
  uint32_t windowID = SDL_GetWindowID(data->sdlWindow);

  switch (msg) {
  case WM_KEYDOWN:
  case WM_SYSKEYDOWN:
  case WM_KEYUP:
    logI("WM_KEYUP: vk=%llu", wParam);
  case WM_SYSKEYUP: {
    if (wParam == VK_LWIN || wParam == VK_RWIN) break;
    SDL_Event e = {};
    e.type =
      (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) ? SDL_KEYDOWN : SDL_KEYUP;
    e.key.windowID = windowID;
    e.key.state = (e.type == SDL_KEYDOWN) ? SDL_PRESSED : SDL_RELEASED;
    e.key.repeat = (e.type == SDL_KEYDOWN && (lParam & (1 << 30))) ? 1 : 0;
    e.key.keysym.scancode = VKToSDLScancode(wParam); // wParam = VK code
    e.key.keysym.sym = SDL_GetKeyFromScancode(e.key.keysym.scancode);
    e.key.keysym.mod = (Uint16)SDL_GetModState();
    SDL_PushEvent(&e);
    return 0;
  }
  case WM_CHAR:
  case WM_SYSCHAR: {
    if (wParam < 0x20) break; // skip control characters
    wchar_t wch = (wchar_t)wParam;
    char utf8[5] = {};
    WideCharToMultiByte(CP_UTF8, 0, &wch, 1, utf8, sizeof(utf8) - 1, nullptr,
                        nullptr);

    SDL_Event e = {};
    e.type = SDL_TEXTINPUT;
    e.text.windowID = windowID;
    SDL_strlcpy(e.text.text, utf8, sizeof(e.text.text));
    SDL_PushEvent(&e);
    return 0;
  }
  case WM_SETFOCUS: {
    if (!data->hasFocus) {
      data->hasFocus = true;
      SDL_Event e = {};
      e.type = SDL_WINDOWEVENT;
      e.window.windowID = windowID;
      e.window.event = SDL_WINDOWEVENT_FOCUS_GAINED;
      SDL_PushEvent(&e);
    }
    break;
  }

  case WM_KILLFOCUS: {
    HWND gaining = (HWND)wParam;
    if (gaining == nullptr) {
      if (data->hasFocus) // only reclaim if we legitimately held focus
        PostMessage(hwnd, WM_RECLAIM_FOCUS, 0, 0);
      break; // never push focus-lost SDL event for nil gaining
    }
    bool dawReclaiming = IsAncestor(gaining, hwnd);
    if (!dawReclaiming && data->hasFocus) {
      data->hasFocus = false;
      SDL_Event e = {};
      e.type = SDL_WINDOWEVENT;
      e.window.windowID = windowID;
      e.window.event = SDL_WINDOWEVENT_FOCUS_LOST;
      SDL_PushEvent(&e);
    }
    break;
  }
  case WM_RECLAIM_FOCUS:
    SetFocus(hwnd);
    break;
  }
  return DefSubclassProc(hwnd, msg, wParam, lParam);
}

SDL_Scancode VKToSDLScancode(WPARAM vk)
{
  switch (vk) {
  case VK_BACK:
    return SDL_SCANCODE_BACKSPACE;
  case VK_TAB:
    return SDL_SCANCODE_TAB;
  case VK_RETURN:
    return SDL_SCANCODE_RETURN;
  case VK_ESCAPE:
    return SDL_SCANCODE_ESCAPE;
  case VK_SPACE:
    return SDL_SCANCODE_SPACE;
  case VK_PRIOR:
    return SDL_SCANCODE_PAGEUP;
  case VK_NEXT:
    return SDL_SCANCODE_PAGEDOWN;
  case VK_END:
    return SDL_SCANCODE_END;
  case VK_HOME:
    return SDL_SCANCODE_HOME;
  case VK_LEFT:
    return SDL_SCANCODE_LEFT;
  case VK_UP:
    return SDL_SCANCODE_UP;
  case VK_RIGHT:
    return SDL_SCANCODE_RIGHT;
  case VK_DOWN:
    return SDL_SCANCODE_DOWN;
  case VK_INSERT:
    return SDL_SCANCODE_INSERT;
  case VK_DELETE:
    return SDL_SCANCODE_DELETE;
  case VK_LSHIFT:
    return SDL_SCANCODE_LSHIFT;
  case VK_RSHIFT:
    return SDL_SCANCODE_RSHIFT;
  case VK_LCONTROL:
    return SDL_SCANCODE_LCTRL;
  case VK_RCONTROL:
    return SDL_SCANCODE_RCTRL;
  case VK_LMENU:
    return SDL_SCANCODE_LALT;
  case VK_RMENU:
    return SDL_SCANCODE_RALT;
  case VK_LWIN:
    return SDL_SCANCODE_LGUI;
  case VK_RWIN:
    return SDL_SCANCODE_RGUI;
  case VK_CAPITAL:
    return SDL_SCANCODE_CAPSLOCK;
  case VK_F1:
    return SDL_SCANCODE_F1;
  case VK_F2:
    return SDL_SCANCODE_F2;
  case VK_F3:
    return SDL_SCANCODE_F3;
  case VK_F4:
    return SDL_SCANCODE_F4;
  case VK_F5:
    return SDL_SCANCODE_F5;
  case VK_F6:
    return SDL_SCANCODE_F6;
  case VK_F7:
    return SDL_SCANCODE_F7;
  case VK_F8:
    return SDL_SCANCODE_F8;
  case VK_F9:
    return SDL_SCANCODE_F9;
  case VK_F10:
    return SDL_SCANCODE_F10;
  case VK_F11:
    return SDL_SCANCODE_F11;
  case VK_F12:
    return SDL_SCANCODE_F12;
  case VK_OEM_1:
    return SDL_SCANCODE_SEMICOLON;
  case VK_OEM_PLUS:
    return SDL_SCANCODE_EQUALS;
  case VK_OEM_COMMA:
    return SDL_SCANCODE_COMMA;
  case VK_OEM_MINUS:
    return SDL_SCANCODE_MINUS;
  case VK_OEM_PERIOD:
    return SDL_SCANCODE_PERIOD;
  case VK_OEM_2:
    return SDL_SCANCODE_SLASH;
  case VK_OEM_3:
    return SDL_SCANCODE_GRAVE;
  case VK_OEM_4:
    return SDL_SCANCODE_LEFTBRACKET;
  case VK_OEM_5:
    return SDL_SCANCODE_BACKSLASH;
  case VK_OEM_6:
    return SDL_SCANCODE_RIGHTBRACKET;
  case VK_OEM_7:
    return SDL_SCANCODE_APOSTROPHE;
  default:
    // A–Z: VK codes 65–90 map linearly to SDL_SCANCODE_A–Z (4–29)
    if (vk >= 'A' && vk <= 'Z')
      return (SDL_Scancode)(SDL_SCANCODE_A + (vk - 'A'));
    // 0–9: VK codes 48–57
    if (vk >= '0' && vk <= '9')
      return (SDL_Scancode)(SDL_SCANCODE_0 + (vk - '0'));
    return SDL_SCANCODE_UNKNOWN;
  }
}

bool IsAncestor(HWND ancestor, HWND child)
{
  HWND cur = GetParent(child);
  while (cur != nullptr) {
    if (cur == ancestor) return true;
    cur = GetParent(cur);
  }
  return false;
}