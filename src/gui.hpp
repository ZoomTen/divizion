#pragma once

/* Main GUI class. */

#include "SDL_video.h"
#include "imgui.h"
#include "src/interface_actions.hpp"
#include <cstdint>
#include <unordered_map>

struct Gui {
  // methods
  Gui(SDL_Window* w, DivizionActions* act);
  ~Gui();
  void RenderGui(void);

  // intrinsic things
  ImGuiContext* c{ nullptr };
  SDL_Window* w{ nullptr };
  DivizionActions* act{ nullptr };

  // dialogs
  bool showAbout{ false };
  bool showDebug{ false };

  // states
  ActiveItemType currentlyViewingType{ INSTRUMENT };
  ActiveItemType selectedType{ NONE };
  int selectedIndex{ -1 };
  int instSelected{ -1 };
  int waveSelected{ -1 };
  int sampSelected{ -1 };
  int lastAssetType{ NONE };
  bool channelsOpen{ false };
  bool isActive{ false };

  // window tracking
  uint32_t wid;
  static std::unordered_map<uint32_t, Gui*> windows;
};