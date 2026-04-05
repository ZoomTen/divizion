#include "../gui.hpp"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include "src/engine/engine.h"
#include "src/impl_actions.hpp"
#include "src/ta-log.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

static DivEngine* newEngine(void);
static int loadFile(DivEngine* e, String path);

int main(int argc, char* argv[])
{
  SDL_Window* w= nullptr;
  SDL_Event e;
  bool run= false;

  initLog(stdout);
  DivEngine* de= newEngine();
  int lf= loadFile(de, "cerulean.fur");
  if (lf) {
    finishLogFile();
    return lf;
  }

  DivizionActionsImpl aimpl= DivizionActionsImpl(de);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "can't init video: %s", SDL_GetError());
    return 1;
  }

  w= SDL_CreateWindow("divizion", 0, 0, 500, 500, 0);
  if (!w) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "can't start window: %s", SDL_GetError());
    return 1;
  }

  Gui g= Gui(w, &aimpl);

  run= true;
  while (run) {
    while (SDL_PollEvent(&e)) {
      uint32_t wid= 0;

      if (e.type == SDL_QUIT) {
        run= false;
        continue;
      }

      // get window ID
      if (e.type == SDL_WINDOWEVENT)
        wid= e.window.windowID;
      else if (e.type >= SDL_MOUSEMOTION && e.type <= SDL_MOUSEWHEEL)
        wid= e.button.windowID;
      else if (e.type >= SDL_KEYDOWN && e.type <= SDL_TEXTINPUT)
        wid= e.key.windowID;

      // apply events
      if (Gui::windows.count(wid)) {
        Gui* active= Gui::windows[wid];
        ImGui::SetCurrentContext(active->c);
        ImGuiIO& io= ImGui::GetIO();
        switch (e.type) {
        case SDL_MOUSEMOTION:
          io.AddMousePosEvent((float)e.motion.x, (float)e.motion.y);
          break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
          io.AddMouseButtonEvent(e.button.button - 1, e.type == SDL_MOUSEBUTTONDOWN);
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

    // render the thing regardless of event
    for (auto& i : Gui::windows) {
      Gui* active= i.second;
      active->RenderGui();
    }
  }

  finishLogFile();
  SDL_DestroyWindow(w);
  SDL_Quit();
  return 0;
}

DivEngine* newEngine(void)
{
  DivEngine* e= new DivEngine();

  if (!e->prePreInit())
    logE("engine stage -2 init failed");
  else
    logI("prepreinit OK");

  e->setAudio(DIV_AUDIO_DUMMY);
  e->setView(DIV_STATUS_NOTHING);

  if (!e->preInit())
    logE("engine stage -1 init failed");
  else
    logI("preinit OK");

  if (!e->init())
    logE("engine stage 0 init failed");
  else
    logI("init OK");

  // e->createNew("id0=4", "Game Boy", false);
  return e;
}

// required for DivEngine
void reportError(String what)
{
  logE("%s", what);
}

int loadFile(DivEngine* e, String path)
{
  if (!path.empty()) {
    logI("loading module...");
    FILE* f= fopen(path.c_str(), "rb");
    if (f == NULL) {
      perror("error");
      return 1;
    }
    if (fseek(f, 0, SEEK_END) < 0) {
      perror("size error");
      fmt::sprintf(_("on seek: %s"), strerror(errno));
      fclose(f);
      return 1;
    }
    ssize_t len= ftell(f);
    if (len == (SIZE_MAX >> 1)) {
      perror("could not get file length");
      fmt::sprintf(_("on pre tell: %s"), strerror(errno));
      fclose(f);
      return 1;
    }
    if (len < 1) {
      if (len == 0) {
        logE("that file is empty!");
      } else {
        perror("tell error");
        fmt::sprintf(_("on tell: %s"), strerror(errno));
      }
      fclose(f);
      return 1;
    }
    if (fseek(f, 0, SEEK_SET) < 0) {
      perror("size error");
      fmt::sprintf(_("on get size: %s"), strerror(errno));
      fclose(f);
      return 1;
    }
    unsigned char* file= new unsigned char[len];
    if (fread(file, 1, (size_t)len, f) != (size_t)len) {
      perror("read error");
      fmt::sprintf(_("on read: %s"), strerror(errno));
      fclose(f);
      delete[] file;
      return 1;
    }
    fclose(f);
    if (!e->load(file, (size_t)len, path.c_str())) {
      e->getLastError();
      logE("could not open file!");
      return 1;
    }
  }
  return 0;
}