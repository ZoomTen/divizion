#include "SDL.h"
#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_video.h"
#include "src/impl_actions.hpp"
#include "src/ta-log.h"
#include "vst.hpp"
#include "../divizion.hpp"
#include "../gui.hpp"
#include <cstdint>
#include <cstring>
#include <windef.h>
#include <windows.h>

static int32_t canDo(const char* name);

extern int32_t processEvents(Vst::AEffect* effect, Vst::VstEvents* e);
extern int32_t setupPresetCopyToHost(Vst::AEffect* effect, void** dest,
                                     bool isAPreset);
extern int32_t setupPresetLoadFromHost(Vst::AEffect* effect, void* source,
                                       size_t size, bool isAPreset);

static std::unordered_map<Divizion*, Gui*> effEditInstances = {};

bool globalGuiInit = false;

Vst::ERect windowSize = {
  0, 0,
  500, 500
};

intptr_t dispatcher(Vst::AEffect* effect, Vst::VstOpcodeToPlugin opcode,
                    int32_t index, intptr_t value, void* ptr, float opt)
{
  Divizion* di = (Divizion*)effect->object;
  intptr_t result = 0;
  switch (opcode) {
  case Vst::effCanDo:
    result = canDo((const char*)ptr);
    break;
  case Vst::effGetChunk: // copy plugin data from plugin to host
    result = setupPresetCopyToHost(effect, (void**)ptr, index == 1);
    break;
  case Vst::effSetChunk: // copy plugin data from host to plugin
    result = setupPresetLoadFromHost(effect, ptr, (size_t)value, index == 1);
    break;
  case Vst::effProcessEvents:
    result = processEvents(effect, (Vst::VstEvents*)ptr);
    break;
  case Vst::effEditOpen:
    if (di) {
      if (!globalGuiInit) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
          logE("can't init SDL video %s", SDL_GetError());
          break;
        }
        globalGuiInit = true;
      }
      HWND parent = (HWND)ptr;
      HWND cwindow = CreateWindowExA(
        0, "STATIC", "",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | SS_NOTIFY,
        0, 0, 500, 500, parent, nullptr, nullptr, nullptr);
      if (!cwindow) {
        logE("can't init own window");
        break;
      }
      SDL_Window* win = SDL_CreateWindowFrom(cwindow);
      if (!win) {
        logE("can't init SDL window from own window");
        break;
      }
      DivizionActionsImpl* i = new DivizionActionsImpl(di->e);
      Gui* g = new Gui(win, i);
      effEditInstances[di] = g;
    }
    break;
  case Vst::effEditGetRect:
    if (di) {
      Vst::ERect** p = (Vst::ERect**)ptr;
      *p = &windowSize;
    }
    break;
  case Vst::effEditIdle:
    if (di && effEditInstances.count(di)) {
      SDL_Event e;
      uint32_t wid = 0;
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_WINDOWEVENT) wid = e.window.windowID;
        else if (e.type >= SDL_MOUSEMOTION && e.type <= SDL_MOUSEWHEEL)
          wid = e.button.windowID;
        else if (e.type >= SDL_KEYDOWN && e.type <= SDL_TEXTINPUT)
          wid = e.key.windowID;
        if (Gui::windows.count(wid)) {
          Gui* active = Gui::windows[wid];
          ImGui::SetCurrentContext(active->c);
          ImGuiIO& io = ImGui::GetIO();
          switch (e.type) {
          case SDL_MOUSEMOTION:
            io.AddMousePosEvent((float)e.motion.x, (float)e.motion.y);
            break;
          case SDL_MOUSEBUTTONDOWN:
          case SDL_MOUSEBUTTONUP:
            io.AddMouseButtonEvent(e.button.button - 1,
                                   e.type == SDL_MOUSEBUTTONDOWN);
            break;
          case SDL_MOUSEWHEEL:
            io.AddMouseWheelEvent(e.wheel.x, e.wheel.y);
            break;
          case SDL_KEYDOWN:
          case SDL_TEXTEDITING:
          case SDL_TEXTINPUT:
            io.AddInputCharacter(e.key.keysym.sym);
            break;
          }
        }
      }
      effEditInstances[di]->RenderGui();
    }
    break;
  case Vst::effEditClose:
    if (di && effEditInstances.count(di)) {
      SDL_Window *w = effEditInstances[di]->w;
      delete effEditInstances[di];
      SDL_DestroyWindow(w);
    }
  case Vst::effGetVstVersion:
    result = (uint32_t)Vst::kVstVersion;
    break;
  case Vst::effGetNumMidiInputChannels:
    result = 16;
    break;
  case Vst::effGetNumMidiOutputChannels:
    result = 0;
    break;
  case Vst::effSetSampleRate:
    // engine.got / engine.want <- has the audio sample rate
    // need to get that...
    break;
  }
  return result;
}

#define yes_i_can 1
#define i_dunno 0
#define no_i_cannot -1
int32_t canDo(const char* name)
{
  // logV("requesting capability: %s", name);
  if (strcmp(name, "receiveVstEvents")) return yes_i_can;
  if (strcmp(name, "receiveVstMidiEvent")) return yes_i_can;
  return no_i_cannot;
}