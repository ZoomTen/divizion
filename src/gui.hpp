#pragma once

#include "vst.hpp"
#include "SDL_events.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include <windef.h>

struct Gui
{
  Gui(Vst::AEffect *effect);
  ~Gui();

  bool open(void *p);
  bool getRect(Vst::ERect **r);
  void idle();

  void RenderGui();
  bool init{false};

  Vst::AEffect *effect;

  // SDL stuff
  SDL_Event event{};
  SDL_Window *window{nullptr};
  SDL_Renderer *renderer{nullptr};

  // Windows stuff
  HWND myWindow{nullptr};
};