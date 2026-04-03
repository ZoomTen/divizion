#pragma once

/* Interface to anything that a GUI needs that can
   be mocked up, plugged-in, or whatever. */

#include "src/engine/instrument.h"
#include "src/engine/sample.h"
#include "src/engine/wavetable.h"
#include <string>
#include <vector>

using std::vector;

typedef enum { NONE = -1, INSTRUMENT, WAVETABLE, SAMPLE } ActiveItemType;

struct DivizionActions {
  // ImGui commands to draw the editor for the specified instrument.
  // This will be an entire panel's worth.
  virtual void drawInstrumentInfo(ActiveItemType type, int index) = 0;

  // What to do when clicking the Add button
  virtual void actAdd(ActiveItemType type) = 0;

  // What to do when clicking the Duplicate button
  virtual void actDuplicate(ActiveItemType type, int index) = 0;

  // What to do when clicking the Load button
  virtual void actLoad(ActiveItemType type) = 0;

  // What to do when clicking the Save button
  virtual void actSave(ActiveItemType type, int index) = 0;

  // What to do when clicking the Move Up button
  virtual void actMoveUp(ActiveItemType type, int index) = 0;

  // What to do when clicking the Move Down button
  virtual void actMoveDown(ActiveItemType type, int index) = 0;

  // What to do when clicking the Delete button
  virtual void actDelete(ActiveItemType type, int index) = 0;

  virtual vector<DivInstrument*> getInstrumentList() = 0;
  virtual vector<DivWavetable*> getWavetables() = 0;
  virtual vector<DivSample*> getSamples() = 0;

  virtual void drawChipInfo() = 0;
  virtual void drawChanInfo() = 0;

  virtual void panic() = 0;
  
  std::string guiErrorMessage;

  virtual ~DivizionActions() = default;
};
