
#include "IconsFontAwesome4.h"
#include "const.hpp"
#include "furnace_gui_utils.hpp"
#include "furnace_macro_def.hpp"
#include "furnace_widgets.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"
#include "src/engine/engine.h"
#include "src/engine/instrument.h"
#include "src/impl_actions.hpp"

const char* gbHWSeqCmdTypes[6] = { _N("Envelope"), _N("Sweep"),
                                   _N("Wait"),     _N("Wait for Release"),
                                   _N("Loop"),     _N("Loop until Release") };

static void drawGbInfo(DivizionActionsImpl* self, int index, DivEngine* e,
                       DivInstrument* ins);
extern void drawMacros(DivizionActionsImpl* self,
                       std::vector<FurnaceGUIMacroDesc>& macros,
                       FurnaceGUIMacroEditState& state, DivInstrument* ins);

String macroHoverNote(int id, float val, void* u)
{
  int* macroVal = (int*)u;
  if ((macroVal[id] & 0xc0000000) == 0x40000000
      || (macroVal[id] & 0xc0000000) == 0x80000000) {
    if (val < -60 || val >= 120) return "???";
    return fmt::sprintf("%d: %s", id, noteNames[(int)val + 60]);
  }
  return fmt::sprintf("%d: %d", id, (int)val);
}

String macroHover(int id, float val, void* u)
{
  return fmt::sprintf("%d: %d", id, (int)val);
}

const char* panBits[5] = { _N("right"), _N("left"), _N("rear right"),
                           _N("rear left"), NULL };
const char* macroAbsoluteMode = "Fixed";
const char* macroRelativeMode = "Relative";
const char* macroQSoundMode = "QSound";
const char* macroDummyMode = "Bug";
void drawInfo(DivizionActionsImpl* self, int index, DivEngine* e,
              DivInstrument* ins)
{
  std::vector<FurnaceGUIMacroDesc> macroList;

  if (ImGui::BeginTable("InsProp", 3)) {
    ImGui::TableSetupColumn("c0", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("c1", ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(72.0f);
    ImGui::Text(_("Name"));

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::PushID(2 + index);
    ImGui::InputText("##Name", &ins->name, ins->name.size());
    ImGui::PopID();

    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    ImGui::Text(_("Type"));

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    int insType = ins->type;
    bool warnType = true;
    for (DivInstrumentType i : e->getPossibleInsTypes()) {
      if (i == insType) warnType = false;
    }
    // pushWarningColor(warnType, warnType && failedNoteOn);
    if (ImGui::BeginCombo("##Type", (insType >= DIV_INS_MAX)
                                      ? _("Unknown")
                                      : _(insTypes[insType][0]))) {
      std::vector<DivInstrumentType> insTypeList;
      for (int i = 0; insTypes[i][0]; i++) {
        insTypeList.push_back((DivInstrumentType)i);
      }
      for (DivInstrumentType i : insTypeList) {
        if (ImGui::Selectable(insTypes[i][0], insType == i)) {
          ins->type = i;

          // reset macro zoom
          memset(ins->temp.vZoom, -1, sizeof(ins->temp.vZoom));
        }
      }
      ImGui::EndCombo();
    } else if (warnType) {
      if (ImGui::IsItemHovered())
        ImGui::SetTooltip("no chips can play\nthis instrument");
    }
    // popWarningColor();
    ImGui::EndTable();
  }

  if (ImGui::BeginTabBar("insEditTab")) {
    // chip specific
    if (ins->type == DIV_INS_GB) {
      if (ImGui::BeginTabItem("Game Boy")) {
        drawGbInfo(self, index, e, ins);
        ImGui::EndTabItem();
      }
    }

    // macros
    if (ins->type < DIV_INS_MAX)
      if (ImGui::BeginTabItem(_("Macros"))) {
        // NEW CODE
        // this is only the first part of an insEdit refactor.
        // don't complain yet!
        int waveCount = MAX(1, e->song.waveLen - 1);

        switch (ins->type) {
        case DIV_INS_STD:
          macroList.push_back(
            FurnaceGUIMacroDesc(_("Volume"), &ins->std.volMacro, 0, 15, 160,
                                uiColors[GUI_COLOR_MACRO_VOLUME]));
          macroList.push_back(FurnaceGUIMacroDesc(
            _("Arpeggio"), &ins->std.arpMacro, -120, 120, 160,
            uiColors[GUI_COLOR_MACRO_PITCH], true, NULL, macroHoverNote, false,
            NULL, true, ins->std.arpMacro.val));
          macroList.push_back(
            FurnaceGUIMacroDesc(_("Noise Mode"), &ins->std.dutyMacro, 0, 3, 160,
                                uiColors[GUI_COLOR_MACRO_NOISE]));
          macroList.push_back(FurnaceGUIMacroDesc(
            _("Panning"), &ins->std.panLMacro, 0, 2, 32,
            uiColors[GUI_COLOR_MACRO_OTHER], false, NULL, NULL, true, panBits));
          macroList.push_back(FurnaceGUIMacroDesc(
            _("Pitch"), &ins->std.pitchMacro, -2048, 2047, 160,
            uiColors[GUI_COLOR_MACRO_PITCH], true, macroRelativeMode));
          macroList.push_back(FurnaceGUIMacroDesc(
            _("Phase Reset"), &ins->std.phaseResetMacro, 0, 1, 32,
            uiColors[GUI_COLOR_MACRO_OTHER], false, NULL, NULL, true));
          break;
        case DIV_INS_GB:
          if (ins->gb.softEnv) {
            macroList.push_back(
              FurnaceGUIMacroDesc(_("Volume"), &ins->std.volMacro, 0, 15, 160,
                                  uiColors[GUI_COLOR_MACRO_VOLUME]));
          }
          macroList.push_back(FurnaceGUIMacroDesc(
            _("Arpeggio"), &ins->std.arpMacro, -120, 120, 160,
            uiColors[GUI_COLOR_MACRO_PITCH], true, NULL, macroHoverNote, false,
            NULL, true, ins->std.arpMacro.val));
          macroList.push_back(
            FurnaceGUIMacroDesc(_("Duty/Noise"), &ins->std.dutyMacro, 0, 3, 160,
                                uiColors[GUI_COLOR_MACRO_NOISE]));
          macroList.push_back(FurnaceGUIMacroDesc(
            _("Waveform"), &ins->std.waveMacro, 0, waveCount, 160,
            uiColors[GUI_COLOR_MACRO_WAVE], false, NULL, NULL, false, NULL));
          macroList.push_back(FurnaceGUIMacroDesc(
            _("Panning"), &ins->std.panLMacro, 0, 2, 32,
            uiColors[GUI_COLOR_MACRO_OTHER], false, NULL, NULL, true, panBits));
          macroList.push_back(FurnaceGUIMacroDesc(
            _("Pitch"), &ins->std.pitchMacro, -2048, 2047, 160,
            uiColors[GUI_COLOR_MACRO_PITCH], true, macroRelativeMode));
          macroList.push_back(FurnaceGUIMacroDesc(
            _("Phase Reset"), &ins->std.phaseResetMacro, 0, 1, 32,
            uiColors[GUI_COLOR_MACRO_OTHER], false, NULL, NULL, true));
          break;
        case DIV_INS_MAX:
        case DIV_INS_NULL:
          break;
        }

        drawMacros(self, macroList, self->macroEditStateMacros, ins);
        ImGui::EndTabItem();
      }
    ImGui::EndTabBar();
  }
}

void drawGbInfo(DivizionActionsImpl* self, int index, DivEngine* e,
                DivInstrument* ins)
{
  ImGui::Checkbox("Use software envelope", &ins->gb.softEnv);
  ImGui::Checkbox("Initialize envelope on every note", &ins->gb.alwaysInit);
  ImGui::Checkbox("Double wave length (GBA only)", &ins->gb.doubleWave);

  ImGui::BeginDisabled(ins->gb.softEnv);
  if (ImGui::BeginTable("GBParams", 2)) {
    ImGui::TableSetupColumn("c0", ImGuiTableColumnFlags_WidthStretch, 0.6f);
    ImGui::TableSetupColumn("c1", ImGuiTableColumnFlags_WidthStretch, 0.4f);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    if (ImGui::BeginTable("GBParamsI", 2)) {
      ImGui::TableSetupColumn("ci0", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("ci1", ImGuiTableColumnFlags_WidthStretch);

      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text(_("Volume"));
      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
      P(CWSliderScalar(self, "##GBVolume", ImGuiDataType_U8, &ins->gb.envVol,
                       &_ZERO, &_FIFTEEN));
      rightClickable

      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text(_("Length"));
      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
      P(CWSliderScalar(self, "##GBEnvLen", ImGuiDataType_U8, &ins->gb.envLen,
                       &_ZERO, &_SEVEN));
      rightClickable

      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text(_("Sound Length"));
      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
      P(CWSliderScalar(self, "##GBSoundLen", ImGuiDataType_U8,
                       &ins->gb.soundLen, &_ZERO, &_SIXTY_FOUR,
                       ins->gb.soundLen > 63 ? _("Infinity") : "%d"));
      rightClickable

      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text(_("Direction"));
      ImGui::TableNextColumn();
      bool goesUp = ins->gb.envDir;
      if (ImGui::RadioButton(_("Up"), goesUp)) {
        goesUp = true;
        ins->gb.envDir = goesUp;
      }
      ImGui::SameLine();
      if (ImGui::RadioButton(_("Down"), !goesUp)) {
        goesUp = false;
        ins->gb.envDir = goesUp;
      }

      ImGui::EndTable();
    }

    ImGui::TableNextColumn();
    drawGBEnv(ins->gb.envVol, ins->gb.envLen, ins->gb.soundLen, ins->gb.envDir,
              ImVec2(ImGui::GetContentRegionAvail().x, 100.0f));

    ImGui::EndTable();
  }

  // todo: add the hw seq thing back in
  ImGui::EndDisabled();
}