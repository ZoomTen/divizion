#include "IconsFontAwesome4.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"
#include "src/furnace_macro_def.hpp"
#include "src/furnace_widgets.hpp"
#include "src/const.hpp"
#include "src/impl_actions.hpp"
#include "src/plot_nolerp.hpp"
#include "src/mml.hpp"
#include <vector>
#include "text_input_widget.hpp"
#include "gui_process_drags.hpp"

static void drawMacroEdit(DivizionActionsImpl* self, FurnaceGUIMacroDesc& i,
                          int totalFit, float availableWidth, int index);

const char* macroTypeLabels[4] = { ICON_FA_BAR_CHART "##IMacroType",
                                   ICON_FUR_ADSR "##IMacroType",
                                   ICON_FUR_TRI "##IMacroType",
                                   ICON_FA_SIGN_OUT "##IMacroType" };
void drawMacros(DivizionActionsImpl* self,
                std::vector<FurnaceGUIMacroDesc>& macros,
                FurnaceGUIMacroEditState& state, DivInstrument* ins)
{
  int index = 0;
  int maxMacroLen = 0;
  float reservedSpace = ImGui::GetStyle().ScrollbarSize;

  for (FurnaceGUIMacroDesc& m : macros) {
    m.ins = ins;
    if (m.macro->len > maxMacroLen) maxMacroLen = m.macro->len;
  }

  if (ImGui::BeginTable("MacroSpace", 2)) {
    float precalcWidth = 0.0f;
    for (FurnaceGUIMacroDesc& i : macros) {
      float next = ImGui::CalcTextSize(i.displayName).x
                   + ImGui::GetStyle().ItemInnerSpacing.x * 2.0f
                   + ImGui::CalcTextSize(ICON_FA_CHEVRON_UP).x
                   + ImGui::GetStyle().ItemSpacing.x * 2.0f;
      if (next > precalcWidth) precalcWidth = next;
    }
    ImGui::TableSetupColumn("c0", ImGuiTableColumnFlags_WidthFixed,
                            MAX(120.0f, precalcWidth));
    ImGui::TableSetupColumn("c1", ImGuiTableColumnFlags_WidthStretch, 0.0);
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    float lenAvail = ImGui::GetContentRegionAvail().x;

    ImGui::SetNextItemWidth(120.0f);
    if (ImGui::InputInt("##MacroPointSize", &self->macroPointSize, 1, 4)) {
      if (self->macroPointSize < 1) self->macroPointSize = 1;
      if (self->macroPointSize > 256) self->macroPointSize = 256;
    }

    ImGui::TableNextColumn();
    float availableWidth = ImGui::GetContentRegionAvail().x - reservedSpace;
    int totalFit = MIN(255, availableWidth / MAX(1, self->macroPointSize));
    int scrollMax = 0;

    for (FurnaceGUIMacroDesc& i : macros) {
      if (i.macro->len > scrollMax) scrollMax = i.macro->len;
    }
    scrollMax -= totalFit;
    if (scrollMax < 0) scrollMax = 0;
    if (self->macroDragScroll > scrollMax) {
      self->macroDragScroll = scrollMax;
    }
    ImGui::BeginDisabled(scrollMax < 1);
    ImGui::SetNextItemWidth(availableWidth);
    if (CWSliderInt(self, "##MacroScroll", &self->macroDragScroll, 0, scrollMax,
                    "")) {
      if (self->macroDragScroll < 0) self->macroDragScroll = 0;
      if (self->macroDragScroll > scrollMax) self->macroDragScroll = scrollMax;
    }
    ImGui::EndDisabled();

    // draw macros
    for (FurnaceGUIMacroDesc& i : macros) {
      ImGui::PushID(index);
      ImGui::TableNextRow();

      // description
      ImGui::TableNextColumn();
      ImGui::Text("%s", i.displayName);
      ImGui::SameLine();
      if (ImGui::SmallButton((i.macro->open & 1)
                               ? (ICON_FA_CHEVRON_UP "##IMacroOpen")
                               : (ICON_FA_CHEVRON_DOWN "##IMacroOpen"))) {
        i.macro->open ^= 1;
      }
      if (i.macro->open & 1) {
        if ((i.macro->open & 6) == 0) {
          ImGui::SetNextItemWidth(lenAvail);
          int macroLen = i.macro->len;
          if (ImGui::InputScalar("##IMacroLen", ImGuiDataType_U8, &macroLen,
                                 &_ONE, &_THREE)) {
            if (macroLen < 0) macroLen = 0;
            if (macroLen > 255) macroLen = 255;
            i.macro->len = macroLen;
          }
        }
        BUTTON_TO_SET_MODE(ImGui::Button);
        ImGui::SameLine();
        BUTTON_TO_SET_PROPS(i);
        if ((i.macro->open & 6) == 0) {
          ImGui::SameLine();
          BUTTON_TO_SET_RELEASE(ImGui::Button);
        }
        // do not change this!
        // anything other than a checkbox will look ugly!
        // if you really need more than two macro modes please tell me.
        if (i.modeName != NULL) {
          bool modeVal = i.macro->mode;
          String modeName = fmt::sprintf("%s##IMacroMode", i.modeName);
          if (ImGui::Checkbox(modeName.c_str(), &modeVal)) {
            i.macro->mode = modeVal;
          }
        }
      }

      // macro area
      ImGui::TableNextColumn();
      drawMacroEdit(self, i, totalFit, availableWidth, index);
      ImGui::PopID();
      index++;
    }

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::TableNextColumn();
    ImGui::BeginDisabled(scrollMax < 1);
    ImGui::SetNextItemWidth(availableWidth);
    if (CWSliderInt(self, "##MacroScroll", &self->macroDragScroll, 0, scrollMax,
                    "")) {
      if (self->macroDragScroll < 0) self->macroDragScroll = 0;
      if (self->macroDragScroll > scrollMax) self->macroDragScroll = scrollMax;
    }
    ImGui::EndDisabled();
    ImGui::EndTable();
  }
}

inline int deBit30(const int val)
{
  if ((val & 0xc0000000) == 0x40000000 || (val & 0xc0000000) == 0x80000000)
    return val ^ 0x40000000;
  return val;
}

inline bool enBit30(const int val)
{
  if ((val & 0xc0000000) == 0x40000000 || (val & 0xc0000000) == 0x80000000)
    return true;
  return false;
}

String macroHoverLoop(int id, float val, void* u)
{
  if (val > 1) return _("Release");
  if (val > 0) return _("Loop");
  return "";
}

String macroHoverBit30(int id, float val, void* u)
{
  if (val > 0) return _("Fixed");
  return _("Relative");
}

String genericGuide(float value)
{
  return fmt::sprintf("%d", (int)value);
}

#define MACRO_VZOOM i.ins->temp.vZoom[i.macro->macroType]
#define MACRO_VSCROLL i.ins->temp.vScroll[i.macro->macroType]

void drawMacroEdit(DivizionActionsImpl* self, FurnaceGUIMacroDesc& i,
                   int totalFit, float availableWidth, int index)
{
  static float asFloat[256];
  static int asInt[256];
  static float loopIndicator[256];
  static float bit30Indicator[256];
  static bool doHighlight[256];

  if ((i.macro->open & 6) == 0) {
    for (int j = 0; j < 256; j++) {
      bit30Indicator[j] = 0;
      if (j + self->macroDragScroll >= i.macro->len) {
        asFloat[j] = 0;
        asInt[j] = 0;
      } else {
        asFloat[j] = deBit30(i.macro->val[j + self->macroDragScroll]);
        asInt[j] = deBit30(i.macro->val[j + self->macroDragScroll]);
        if (i.bit30)
          bit30Indicator[j] = enBit30(i.macro->val[j + self->macroDragScroll]);
      }
      if (j + self->macroDragScroll >= i.macro->len
          || (j + self->macroDragScroll > i.macro->rel
              && i.macro->loop < i.macro->rel)) {
        loopIndicator[j] = 0;
      } else {
        loopIndicator[j] = ((i.macro->loop != 255
                             && (j + self->macroDragScroll) >= i.macro->loop))
                           | ((i.macro->rel != 255
                               && (j + self->macroDragScroll) == i.macro->rel)
                              << 1);
      }
    }
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

    if (MACRO_VZOOM < 1) {
      if (i.macro->macroType == DIV_MACRO_ARP || i.isArp) {
        MACRO_VZOOM = 24;
        MACRO_VSCROLL = 120 - 12;
      } else if (i.macro->macroType == DIV_MACRO_PITCH || i.isPitch) {
        MACRO_VZOOM = 128;
        MACRO_VSCROLL = 2048 - 64;
      } else {
        MACRO_VZOOM = i.max - i.min;
        MACRO_VSCROLL = 0;
      }
    }
    if (MACRO_VZOOM > (i.max - i.min)) {
      MACRO_VZOOM = i.max - i.min;
    }

    memset(doHighlight, 0, 256 * sizeof(bool));

    if (i.isBitfield) {
      PlotBitfield(
        "##IMacro", asInt, totalFit, 0, i.bitfieldBits, i.max,
        ImVec2(availableWidth, (i.macro->open & 1) ? (i.height) : (32.0f)),
        sizeof(float), doHighlight, uiColors[GUI_COLOR_MACRO_HIGHLIGHT],
        i.color, i.hoverFunc, i.hoverFuncUser);
    } else {
      PlotCustom(
        "##IMacro", asFloat, totalFit, self->macroDragScroll, NULL,
        i.min + MACRO_VSCROLL, i.min + MACRO_VSCROLL + MACRO_VZOOM,
        ImVec2(availableWidth, (i.macro->open & 1) ? (i.height) : (32.0f)),
        sizeof(float), i.color, i.macro->len - self->macroDragScroll,
        i.hoverFunc, i.hoverFuncUser, i.blockMode,
        (i.macro->open & 1) ? genericGuide : NULL, doHighlight,
        uiColors[GUI_COLOR_MACRO_HIGHLIGHT]);
    }
    if ((i.macro->open & 1)
        && (ImGui::IsItemClicked(ImGuiMouseButton_Left)
            || ImGui::IsItemClicked(ImGuiMouseButton_Right))) {
      ImGui::InhibitInertialScroll();
      self->macroDragStart = ImGui::GetItemRectMin();
      self->macroDragAreaSize = ImVec2(availableWidth, i.height);
      if (i.isBitfield) {
        self->macroDragMin = i.min;
        self->macroDragMax = i.max;
      } else {
        self->macroDragMin = i.min + MACRO_VSCROLL;
        self->macroDragMax = i.min + MACRO_VSCROLL + MACRO_VZOOM;
      }
      self->macroDragBitMode = i.isBitfield;
      self->macroDragInitialValueSet = false;
      self->macroDragInitialValue = false;
      self->macroDragLen = totalFit;
      self->macroDragActive = true;
      self->macroDragBit30 = i.bit30;
      self->macroDragSettingBit30 = false;
      self->macroDragTarget = i.macro->val;
      self->macroDragChar = false;
      self->macroDragLineMode =
        (i.isBitfield) ? false : ImGui::IsItemClicked(ImGuiMouseButton_Right);
      self->macroDragLineInitial = ImVec2(0, 0);
      self->lastMacroDesc = i;
      processDrags(self, ImGui::GetMousePos().x, ImGui::GetMousePos().y);
    }
    if ((i.macro->open & 1)) {
      if (ImGui::IsItemHovered()) {
        if (ctrlWheeling) {
          self->macroPointSize += self->wheelY;
          if (self->macroPointSize < 1) self->macroPointSize = 1;
          if (self->macroPointSize > 256) self->macroPointSize = 256;
        } else if ((ImGui::IsKeyDown(ImGuiKey_LeftShift)
                    || ImGui::IsKeyDown(ImGuiKey_RightShift))
                   && self->wheelY != 0) {
          MACRO_VSCROLL += self->wheelY * (1 + (MACRO_VZOOM >> 4));
          if (MACRO_VSCROLL < 0) MACRO_VSCROLL = 0;
          if (MACRO_VSCROLL > ((i.max - i.min) - MACRO_VZOOM))
            MACRO_VSCROLL = (i.max - i.min) - MACRO_VZOOM;
        }
      }

      // slider
      if (!i.isBitfield) {
        ImS64 scrollV = (i.max - i.min - MACRO_VZOOM) - MACRO_VSCROLL;
        ImS64 availV = MACRO_VZOOM;
        ImS64 contentsV = (i.max - i.min);

        ImGui::SameLine(0.0f);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX()
                             - ImGui::GetStyle().ItemSpacing.x);
        ImRect scrollbarPos =
          ImRect(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos());
        scrollbarPos.Max.x += ImGui::GetStyle().ScrollbarSize;
        scrollbarPos.Max.y += i.height;
        ImGui::Dummy(ImVec2(ImGui::GetStyle().ScrollbarSize, i.height));
        if (ImGui::IsItemHovered() && ctrlWheeling) {
          MACRO_VSCROLL += self->wheelY * (1 + (MACRO_VZOOM >> 4));
          if (MACRO_VSCROLL < 0) MACRO_VSCROLL = 0;
          if (MACRO_VSCROLL > ((i.max - i.min) - MACRO_VZOOM))
            MACRO_VSCROLL = (i.max - i.min) - MACRO_VZOOM;
        }

        ImGuiID scrollbarID = ImGui::GetID("##IMacroVScroll");
        ImGui::KeepAliveID(scrollbarID);
        if (ImGui::ScrollbarEx(scrollbarPos, scrollbarID, ImGuiAxis_Y, &scrollV,
                               availV, contentsV, 0)) {
          MACRO_VSCROLL = (i.max - i.min - MACRO_VZOOM) - scrollV;
        }
      }

      // bit 30 area
      if (i.bit30) {
        PlotCustom("##IMacroBit30", bit30Indicator, totalFit,
                   self->macroDragScroll, NULL, 0, 1,
                   ImVec2(availableWidth, 12.0f), sizeof(float), i.color,
                   i.macro->len - self->macroDragScroll, &macroHoverBit30);
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
          ImGui::InhibitInertialScroll();
          self->macroDragStart = ImGui::GetItemRectMin();
          self->macroDragAreaSize = ImVec2(availableWidth, 12.0f);
          self->macroDragInitialValueSet = false;
          self->macroDragInitialValue = false;
          self->macroDragLen = totalFit;
          self->macroDragActive = true;
          self->macroDragBit30 = i.bit30;
          self->macroDragSettingBit30 = true;
          self->macroDragTarget = i.macro->val;
          self->macroDragChar = false;
          self->macroDragLineMode = false;
          self->macroDragLineInitial = ImVec2(0, 0);
          self->lastMacroDesc = i;
          processDrags(self, ImGui::GetMousePos().x, ImGui::GetMousePos().y);
        }
      }

      // loop area
      PlotCustom("##IMacroLoop", loopIndicator, totalFit, self->macroDragScroll,
                 NULL, 0, 2, ImVec2(availableWidth, 12.0f), sizeof(float),
                 i.color, i.macro->len - self->macroDragScroll,
                 &macroHoverLoop);
      if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        ImGui::InhibitInertialScroll();
        self->macroLoopDragStart = ImGui::GetItemRectMin();
        self->macroLoopDragAreaSize = ImVec2(availableWidth, 12.0f);
        self->macroLoopDragLen = totalFit;
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift)
            || ImGui::IsKeyDown(ImGuiKey_RightShift)) {
          self->macroLoopDragTarget = &i.macro->rel;
        } else {
          self->macroLoopDragTarget = &i.macro->loop;
        }
        self->macroLoopDragActive = true;
        processDrags(self, ImGui::GetMousePos().x, ImGui::GetMousePos().y);
      }
      if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        ImGui::InhibitInertialScroll();
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift)
            || ImGui::IsKeyDown(ImGuiKey_RightShift)) {
          i.macro->rel = 255;
        } else {
          i.macro->loop = 255;
        }
      }

#ifdef WIN32
      ImGui::Text("MML");
      ImGui::SameLine();
#endif
      ImGui::SetNextItemWidth(availableWidth);
      String& mmlStr = self->mmlString[index];
      if (InputText("IMacroMML", &mmlStr)) {
        decodeMMLStr(mmlStr, i.macro->val, i.macro->len, i.macro->loop, i.min,
                     (i.isBitfield) ? ((1 << (i.isBitfield ? (i.max) : 0)) - 1)
                                    : i.max,
                     i.macro->rel, i.bit30);
      }
      if (!ImGui::IsItemActive()) {
        encodeMMLStr(mmlStr, i.macro->val, i.macro->len, i.macro->loop,
                     i.macro->rel, false, i.bit30);
      }
    }
    ImGui::PopStyleVar();
  } else {
    if (i.macro->open & 2) {
      const bool compact = (availableWidth < 300.0f);
      if (ImGui::BeginTable("MacroADSR", compact ? 2 : 4)) {
        ImGui::TableSetupColumn("c0", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("c1", ImGuiTableColumnFlags_WidthStretch, 0.3);
        if (!compact) {
          ImGui::TableSetupColumn("c2", ImGuiTableColumnFlags_WidthFixed);
          ImGui::TableSetupColumn("c3", ImGuiTableColumnFlags_WidthStretch,
                                  0.3);
        }
        // ImGui::TableSetupColumn("c4",ImGuiTableColumnFlags_WidthStretch,0.4);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text(_("Bottom"));
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputInt("##MABottom", &i.macro->val[0], 1, 16)) {
          if (i.macro->val[0] < i.min) i.macro->val[0] = i.min;
          if (i.macro->val[0] > i.max) i.macro->val[0] = i.max;
        }

        if (compact) ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text(_("Top"));
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputInt("##MATop", &i.macro->val[1], 1, 16)) {
          if (i.macro->val[1] < i.min) i.macro->val[1] = i.min;
          if (i.macro->val[1] > i.max) i.macro->val[1] = i.max;
        }

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text(_("Attack"));
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (CWSliderInt(self, "##MAAR", &i.macro->val[2], 0, 255)) {
          if (i.macro->val[2] < 0) i.macro->val[2] = 0;
          if (i.macro->val[2] > 255) i.macro->val[2] = 255;
        }
        if (compact) {
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::AlignTextToFramePadding();
          ImGui::Text(_("Hold"));
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
          if (CWSliderInt(self, "##MAHT", &i.macro->val[3], 0, 255)) {
            if (i.macro->val[3] < 0) i.macro->val[3] = 0;
            if (i.macro->val[3] > 255) i.macro->val[3] = 255;
          }

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::AlignTextToFramePadding();
          ImGui::Text(_("Decay"));
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
          if (CWSliderInt(self, "##MADR", &i.macro->val[4], 0, 255)) {
            if (i.macro->val[4] < 0) i.macro->val[4] = 0;
            if (i.macro->val[4] > 255) i.macro->val[4] = 255;
          }

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::AlignTextToFramePadding();
          ImGui::Text(_("Sustain"));
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
          if (CWSliderInt(self, "##MASL", &i.macro->val[5], 0, 255)) {
            if (i.macro->val[5] < 0) i.macro->val[5] = 0;
            if (i.macro->val[5] > 255) i.macro->val[5] = 255;
          }

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::AlignTextToFramePadding();
          ImGui::Text(_("SusTime"));
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
          if (CWSliderInt(self, "##MAST", &i.macro->val[6], 0, 255)) {
            if (i.macro->val[6] < 0) i.macro->val[6] = 0;
            if (i.macro->val[6] > 255) i.macro->val[6] = 255;
          }

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::AlignTextToFramePadding();
          ImGui::Text(_("SusDecay"));
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
          if (CWSliderInt(self, "##MASR", &i.macro->val[7], 0, 255)) {
            if (i.macro->val[7] < 0) i.macro->val[7] = 0;
            if (i.macro->val[7] > 255) i.macro->val[7] = 255;
          }

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::AlignTextToFramePadding();
          ImGui::Text(_("Release"));
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
          if (CWSliderInt(self, "##MARR", &i.macro->val[8], 0, 255)) {
            if (i.macro->val[8] < 0) i.macro->val[8] = 0;
            if (i.macro->val[8] > 255) i.macro->val[8] = 255;
          }

        } else {
          ImGui::TableNextColumn();
          ImGui::AlignTextToFramePadding();
          ImGui::Text(_("Sustain"));
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
          if (CWSliderInt(self, "##MASL", &i.macro->val[5], 0, 255)) {
            if (i.macro->val[5] < 0) i.macro->val[5] = 0;
            if (i.macro->val[5] > 255) i.macro->val[5] = 255;
          }

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::AlignTextToFramePadding();
          ImGui::Text(_("Hold"));
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
          if (CWSliderInt(self, "##MAHT", &i.macro->val[3], 0, 255)) {
            if (i.macro->val[3] < 0) i.macro->val[3] = 0;
            if (i.macro->val[3] > 255) i.macro->val[3] = 255;
          }

          ImGui::TableNextColumn();
          ImGui::AlignTextToFramePadding();
          ImGui::Text(_("SusTime"));
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
          if (CWSliderInt(self, "##MAST", &i.macro->val[6], 0, 255)) {
            if (i.macro->val[6] < 0) i.macro->val[6] = 0;
            if (i.macro->val[6] > 255) i.macro->val[6] = 255;
          }

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::AlignTextToFramePadding();
          ImGui::Text(_("Decay"));
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
          if (CWSliderInt(self, "##MADR", &i.macro->val[4], 0, 255)) {
            if (i.macro->val[4] < 0) i.macro->val[4] = 0;
            if (i.macro->val[4] > 255) i.macro->val[4] = 255;
          }

          ImGui::TableNextColumn();
          ImGui::AlignTextToFramePadding();
          ImGui::Text(_("SusDecay"));
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
          if (CWSliderInt(self, "##MASR", &i.macro->val[7], 0, 255)) {
            if (i.macro->val[7] < 0) i.macro->val[7] = 0;
            if (i.macro->val[7] > 255) i.macro->val[7] = 255;
          }

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::TableNextColumn();

          ImGui::TableNextColumn();
          ImGui::AlignTextToFramePadding();
          ImGui::Text(_("Release"));
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
          if (CWSliderInt(self, "##MARR", &i.macro->val[8], 0, 255)) {
            if (i.macro->val[8] < 0) i.macro->val[8] = 0;
            if (i.macro->val[8] > 255) i.macro->val[8] = 255;
          }
        }

        ImGui::EndTable();
      }
    }
    if (i.macro->open & 4) {
      const bool compact = (availableWidth < 300.0f);
      if (ImGui::BeginTable("MacroLFO", compact ? 2 : 4)) {
        ImGui::TableSetupColumn("c0", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("c1", ImGuiTableColumnFlags_WidthStretch, 0.3);
        if (!compact) {
          ImGui::TableSetupColumn("c2", ImGuiTableColumnFlags_WidthFixed);
          ImGui::TableSetupColumn("c3", ImGuiTableColumnFlags_WidthStretch,
                                  0.3);
        }

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text(_("Bottom"));
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputInt("##MABottom", &i.macro->val[0], 1, 16)) {
          if (i.macro->val[0] < i.min) i.macro->val[0] = i.min;
          if (i.macro->val[0] > i.max) i.macro->val[0] = i.max;
        }

        if (compact) ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text(_("Top"));
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputInt("##MATop", &i.macro->val[1], 1, 16)) {
          if (i.macro->val[1] < i.min) i.macro->val[1] = i.min;
          if (i.macro->val[1] > i.max) i.macro->val[1] = i.max;
        }

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text(_("Speed"));
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (CWSliderInt(self, "##MLSpeed", &i.macro->val[11], 0, 255)) {
          if (i.macro->val[11] < 0) i.macro->val[11] = 0;
          if (i.macro->val[11] > 255) i.macro->val[11] = 255;
        }

        if (compact) ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text(_("Phase"));
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (CWSliderInt(self, "##MLPhase", &i.macro->val[13], 0, 1023)) {
          if (i.macro->val[13] < 0) i.macro->val[13] = 0;
          if (i.macro->val[13] > 1023) i.macro->val[13] = 1023;
        }

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text(_("Shape"));
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (CWSliderInt(self, "##MLShape", &i.macro->val[12], 0, 2,
                        macroLFOShapes[i.macro->val[12] & 3])) {
          if (i.macro->val[12] < 0) i.macro->val[12] = 0;
          if (i.macro->val[12] > 2) i.macro->val[12] = 2;
        }

        ImGui::EndTable();
      }
    }
  }
}
