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
  virtual vector<DivInstrument*> getInstrumentList() = 0;
  virtual vector<DivWavetable*> getWavetables() = 0;
  virtual vector<DivSample*> getSamples() = 0;

  virtual void drawChipInfo() = 0;
  virtual void drawChanInfo() = 0;
  virtual void drawRegView() = 0;

  virtual void loadPrjFile(std::string name) = 0;
  virtual void savePrjFile(std::string name) = 0;

  virtual void panic() = 0;
  
  std::string guiErrorMessage;

  virtual ~DivizionActions() = default;
};
