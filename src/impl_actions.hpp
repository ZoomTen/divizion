#pragma once

#include "imgui.h"
#include "interface_actions.hpp"
#include "src/engine/engine.h"
#include "src/furnace_macro_def.hpp"
#include "src/impl_actions_sys_categories.hpp"

struct DivizionActionsImpl : DivizionActions {
  // core stuff
  DivEngine* e{ nullptr };
  DivizionActionsImpl(DivEngine* e);

  // UI state stuff... :(
  int wheelY{ 0 };
  FurnaceGUIMacroEditState macroEditStateFM{};
  FurnaceGUIMacroEditState macroEditStateOP[4]{};
  FurnaceGUIMacroEditState macroEditStateMacros{};
  FurnaceGUIMacroDesc lastMacroDesc{ NULL, NULL, 0, 0, 0.0 };
  int macroPointSize{ 16 };
  int macroDragScroll{ 0 };
  ImVec2 macroDragStart{ 0, 0 };
  ImVec2 macroDragAreaSize{ 0, 0 };
  unsigned char* macroDragCTarget{ NULL };
  int* macroDragTarget{ NULL };
  int macroDragLen{ 0 };
  int macroDragMin{ 0 };
  int macroDragMax{ 0 };
  int macroDragLastX{ 0 };
  int macroDragLastY{ 0 };
  bool macroDragBitMode{ false };
  bool macroDragInitialValueSet{ false };
  bool macroDragInitialValue{ false };
  bool macroDragChar{ false };
  bool macroDragBit30{ false };
  bool macroDragSettingBit30{ false };
  bool macroDragLineMode{ false };
  bool macroDragMouseMoved{ false };
  ImVec2 macroDragLineInitial{ 0, 0 };
  ImVec2 macroDragLineInitialV{ 0, 0 };
  bool macroDragActive{ false };
  int macroOffX, macroOffY;
  float macroScaleX{ 0.0 };
  float macroScaleY{ 0.0 };
  int macroRandMin{ 0 };
  int macroRandMax{ 0 };
  ImVec2 macroLoopDragStart{ 0, 0 };
  ImVec2 macroLoopDragAreaSize{ 0, 0 };
  unsigned char* macroLoopDragTarget{ NULL };
  int macroLoopDragLen{ 0 };
  bool macroLoopDragActive{ false };
  ImVec2 waveDragStart{ 0, 0 };
  ImVec2 waveDragAreaSize{ 0, 0 };
  int* waveDragTarget{ NULL };
  int waveDragLen{ 0 };
  int waveDragMin{ 0 };
  int waveDragMax{ 0 };
  bool waveDragActive{ false };
  String mmlString[32]{""};
  bool preserveChanPos{false};
  bool sysDupCloneChannels{false};
  bool sysDupEnd{false};
  int sysToMove{-1};
  int sysToDelete{-1};
  bool snesFilterHex{false};
  String mmlStringSNES{""};
  std::vector<FurnaceGUISysCategory> sysCategories;
  void initCategories();
  const int* curSysSection;
  String sysSearchQuery;
  std::vector<DivSystem> sysSearchResults;
  int curWave{0};
  float waveGenInvertPoint{1.0f};
  int waveEditStyle{0};
  bool waveSigned{false};
  bool waveHex{false};
  String mmlStringW;

  // actions
  void drawInstrumentInfo(ActiveItemType type, int index) override;
  void drawChipInfo() override;
  void drawChanInfo() override;
  void actAdd(ActiveItemType type) override;
  void actDuplicate(ActiveItemType type, int index) override;
  void actLoad(ActiveItemType type) override;
  void actSave(ActiveItemType type, int index) override;
  void actMoveUp(ActiveItemType type, int index) override;
  void actMoveDown(ActiveItemType type, int index) override;
  void actDelete(ActiveItemType type, int index) override;
  vector<DivInstrument*> getInstrumentList() override;
  vector<DivWavetable*> getWavetables() override;
  vector<DivSample*> getSamples() override;
  void panic() override;
};

#define _GET(x, y) (y < 0) ? nullptr : (y < x.size()) ? x[y] : nullptr;
