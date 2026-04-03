#include "impl_actions.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "src/engine/instrument.h"
#include "src/engine/sample.h"
#include "src/engine/wavetable.h"
#include "src/interface_actions.hpp"

void drawInfo(DivizionActionsImpl* self, int index, DivEngine* e,
              DivInstrument* i);
void drawInfo(DivizionActionsImpl* self, int index, DivWavetable* i);
void drawInfo(DivSample* i);
void drawInvalidPage(void);

extern void drawWaveEdit(DivizionActionsImpl* self, DivWavetable* i);

DivizionActionsImpl::DivizionActionsImpl(DivEngine* e)
{
  this->e = e;
  this->guiErrorMessage = "";
  this->initCategories();
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
    if (wav) drawInfo(this, index,wav);
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

void DivizionActionsImpl::actAdd(ActiveItemType type)
{
  switch (type) {
  case INSTRUMENT: {
    int curIns = e->addInstrument();
    if (curIns == -1) {
      this->guiErrorMessage = "too many instruments!";
    } else {
      e->song.ins[curIns]->fm.fb = 0;
      for (int i = 0; i < 4; i++) {
        e->song.ins[curIns]->fm.op[i] = DivInstrumentFM::Operator();
        e->song.ins[curIns]->fm.op[i].ar = 31;
        e->song.ins[curIns]->fm.op[i].dr = 31;
        e->song.ins[curIns]->fm.op[i].rr = 15;
        e->song.ins[curIns]->fm.op[i].tl = 127;
        e->song.ins[curIns]->fm.op[i].dt = 3;
        e->song.ins[curIns]->esfm.op[i].ct = 0;
        e->song.ins[curIns]->esfm.op[i].dt = 0;
        e->song.ins[curIns]->esfm.op[i].modIn = 0;
        e->song.ins[curIns]->esfm.op[i].outLvl = 0;
      }
    }
    break;
  }
  case WAVETABLE: {
    std::vector<DivSystem> alreadyDone;
    for (int i = 0; i < e->song.systemLen; i++) {
      bool skip = false;
      for (DivSystem j : alreadyDone) {
        if (e->song.system[i] == j) {
          skip = true;
          break;
        }
      }
      if (skip) continue;
      const DivSysDef* sysDef = e->getSystemDef(e->song.system[i]);
      alreadyDone.push_back(e->song.system[i]);
      if (sysDef == NULL) continue;
      if (sysDef->waveHeight == 0) continue;
    }
    int finalWidth = 32;
    int finalHeight = 32;
    int curWave = e->addWave();
    if (curWave == -1) {
      this->guiErrorMessage = "too many wavetables!";
    } else {
      e->song.wave[curWave]->len = finalWidth;
      e->song.wave[curWave]->max = finalHeight - 1;
      for (int j = 0; j < finalWidth; j++) {
        e->song.wave[curWave]->data[j] = (j * finalHeight) / finalWidth;
      }
    }
    break;
  }
  case SAMPLE:
    int curSample = e->addSample();
    if (curSample == -1) this->guiErrorMessage = "too many samples!";
    break;
  }
}

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

void drawInfo(DivizionActionsImpl* self, int index, DivWavetable* i) {
  self->curWave = index;
  drawWaveEdit(self, i);
}

void drawInfo(DivSample* i) {
  ImGui::Text("drawInfo: TODO");
}

void drawInvalidPage(void)
{
  ImGui::Text("<- select something...");
}

void DivizionActionsImpl::panic()
{
  this->e->syncReset();
}

void DivizionActionsImpl::drawChanInfo()
{
  // TODO: able to map MIDI channels to chip channels
  if (ImGui::BeginTable("SystemList", 2)) {
    // ImGui::TableSetupColumn("c1", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("c2", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("c3", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
    // ImGui::TableNextColumn();
    ImGui::TableNextColumn();
    ImGui::Text("MIDI");
    ImGui::TableNextColumn();
    ImGui::Text("Chip");
    for (int i = 0; i < 16; i++) {
      ImGui::PushID(i);
      ImGui::TableNextRow();
      // ImGui::TableNextColumn();
      // ImGui::Button(ICON_FA_ARROWS);
      ImGui::TableNextColumn();
      ImGui::Text("  %d  ", i+1);
      ImGui::TableNextColumn();
      ImGui::Text("%s #%d", e->getSystemName(e->sysOfChan[i]),e->dispatchChanOfChan[i]);
      ImGui::PopID();
    }
    ImGui::EndTable();
  }
}