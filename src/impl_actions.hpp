#pragma once

#include "imgui.h"
#include "interface_actions.hpp"
#include "src/engine/engine.h"

struct DivizionActionsImpl : DivizionActions {
  // core stuff
  DivEngine* e{ nullptr };
  DivizionActionsImpl(DivEngine* e);

  // UI state stuff... :(
  bool preserveChanPos{false};
  int sysToMove{-1};

  // actions
  void drawChipInfo() override;
  void drawChanInfo() override;
  vector<DivInstrument*> getInstrumentList() override;
  vector<DivWavetable*> getWavetables() override;
  vector<DivSample*> getSamples() override;
  void panic() override;
  void loadPrjFile(std::string name) override;
  void savePrjFile(std::string name) override;
  void drawRegView() override;
};

#define _GET(x, y) (y < 0) ? nullptr : (y < x.size()) ? x[y] : nullptr;
