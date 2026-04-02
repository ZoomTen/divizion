#include "impl_actions.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "src/engine/instrument.h"
#include "src/engine/sample.h"
#include "src/engine/wavetable.h"

void drawInfo(DivizionActionsImpl* self, int index, DivEngine* e,
              DivInstrument* i);
void drawInfo(DivWavetable* i);
void drawInfo(DivSample* i);
void drawInvalidPage(void);

DivizionActionsImpl::DivizionActionsImpl(DivEngine* e)
{
  this->e = e;
}

void DivizionActionsImpl::drawInstrumentInfo(ActiveItemType type, int index)
{
  DivInstrument* ins;
  DivWavetable* wav;
  DivSample* sam;

  switch (type) {
  case INSTRUMENT:
    ins = _GET(this->e->song.ins, index);
    if (ins) drawInfo(this, index, e, ins);
    else drawInvalidPage();
    break;
  case WAVETABLE:
    wav = _GET(this->e->song.wave, index);
    if (wav) drawInfo(wav);
    else drawInvalidPage();
    break;
  case SAMPLE:
    sam = _GET(this->e->song.sample, index);
    if (sam) drawInfo(sam);
    else drawInvalidPage();
    break;
  default:
    drawInvalidPage();
    break;
  }
}

void DivizionActionsImpl::actAdd(ActiveItemType type) {}

void DivizionActionsImpl::actDuplicate(ActiveItemType type, int index) {}

void DivizionActionsImpl::actLoad(ActiveItemType type) {}

void DivizionActionsImpl::actSave(ActiveItemType type, int index) {}

void DivizionActionsImpl::actMoveUp(ActiveItemType type, int index) {}

void DivizionActionsImpl::actMoveDown(ActiveItemType type, int index) {}

void DivizionActionsImpl::actDelete(ActiveItemType type, int index) {}

vector<DivInstrument*> DivizionActionsImpl::getInstrumentList()
{
  return this->e->song.ins;
}

vector<DivWavetable*> DivizionActionsImpl::getWavetables()
{
  return this->e->song.wave;
}

vector<DivSample*> DivizionActionsImpl::getSamples()
{
  return this->e->song.sample;
}

void drawInfo(DivWavetable* i) {}

void drawInfo(DivSample* i) {}

void drawInvalidPage(void)
{
  ImGui::Text("<- select something...");
}
