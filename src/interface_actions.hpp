#pragma once

/* Interface to anything that a GUI needs that can
   be mocked up, plugged-in, or whatever. */

#include <string>
#include <vector>
struct DivizionActions
{
  // ImGui commands to draw the editor for the specified instrument.
  // This will be an entire panel's worth.
  virtual void drawInstrumentInfo(int type, int index) = 0;

  // What to do when clicking the Add button
  virtual void actAdd(int type) = 0;

  // What to do when clicking the Duplicate button
  virtual void actDuplicate(int type, int index) = 0;

  // What to do when clicking the Load button
  virtual void actLoad(int type) = 0;

  // What to do when clicking the Save button
  virtual void actSave(int type, int index) = 0;

  // What to do when clicking the Move Up button
  virtual void actMoveUp(int type, int index) = 0;

  // What to do when clicking the Move Down button
  virtual void actMoveDown(int type, int index) = 0;

  // What to do when clicking the Delete button
  virtual void actDelete(int type, int index) = 0;

  virtual std::vector<std::string> getInstrumentList() = 0;

  virtual ~DivizionActions() = default;
};