#include "SDL.h"
#include "SDL_syswm.h"
#include "src/divizion.hpp"
#include "src/gui.hpp"
#include "src/impl_actions.hpp"

static std::unordered_map<Divizion*, Gui*> effEditInstances = {};
static bool globalGuiInit = false;

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
      /* keyboard events are omitted because VST is just weird with SDL+imgui for some
         reason and i do not know how to deal with it */
      }
    }
  }
  effEditInstances[di]->RenderGui();
}

HWND GetHWND(SDL_Window* sdlWindow)
{
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  if (SDL_GetWindowWMInfo(sdlWindow, &wmInfo)) return wmInfo.info.win.window;
  return NULL;
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