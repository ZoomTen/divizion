#pragma once

#include "IconsFontAwesome4.h"
#include "furIcons.h"
#include "imgui.h"
#include "src/engine/instrument.h"

struct FurnaceGUIMacroDesc {
  DivInstrument* ins;
  DivInstrumentMacro* macro;
  int min, max;
  float height;
  const char* displayName;
  const char** bitfieldBits;
  const char* modeName;
  ImVec4 color;
  bool isBitfield, blockMode, bit30;
  String (*hoverFunc)(int, float, void*);
  void* hoverFuncUser;
  bool isArp;
  bool isPitch;

  FurnaceGUIMacroDesc(const char* name, DivInstrumentMacro* m, int macroMin,
                      int macroMax, float macroHeight,
                      ImVec4 col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                      bool block = false, const char* mName = NULL,
                      String (*hf)(int, float, void*) = NULL,
                      bool bitfield = false, const char** bfVal = NULL,
                      bool bit30Special = false, void* hfu = NULL,
                      bool isArp = false, bool isPitch = false)
    : ins(NULL), macro(m), height(macroHeight), displayName(name),
      bitfieldBits(bfVal), modeName(mName), color(col), isBitfield(bitfield),
      blockMode(block), bit30(bit30Special), hoverFunc(hf), hoverFuncUser(hfu),
      isArp(isArp), isPitch(isPitch)
  {
    // MSVC -> hell
    this->min = macroMin;
    this->max = macroMax;
  }
};

struct FurnaceGUIMacroEditState {
  int selectedMacro;
  FurnaceGUIMacroEditState() : selectedMacro(0) {}
};

#define BUTTON_TO_SET_MODE(buttonType)                                         \
  if (buttonType(macroTypeLabels[(i.macro->open >> 1) & 3])) {                 \
    unsigned char prevOpen = i.macro->open;                                    \
    if (i.macro->open >= 4) {                                                  \
      i.macro->open &= (~6);                                                   \
    } else {                                                                   \
      i.macro->open += 2;                                                      \
    }                                                                          \
                                                                               \
    /* check whether macro type is now ADSR/LFO or sequence */                 \
    if (((prevOpen & 6) ? 1 : 0) != ((i.macro->open & 6) ? 1 : 0)) {           \
      /* swap memory */                                                        \
      /* this way the macro isn't corrupted if the user decides to go */       \
      /* back to sequence mode */                                              \
      i.macro->len ^= i.ins->temp.lenMemory[i.macro->macroType];               \
      i.ins->temp.lenMemory[i.macro->macroType] ^= i.macro->len;               \
      i.macro->len ^= i.ins->temp.lenMemory[i.macro->macroType];               \
                                                                               \
      for (int j = 0; j < 16; j++) {                                           \
        i.macro->val[j] ^= i.ins->temp.typeMemory[i.macro->macroType][j];      \
        i.ins->temp.typeMemory[i.macro->macroType][j] ^= i.macro->val[j];      \
        i.macro->val[j] ^= i.ins->temp.typeMemory[i.macro->macroType][j];      \
      }                                                                        \
                                                                               \
      /* if ADSR/LFO, populate min/max */                                      \
      if (i.macro->open & 6) {                                                 \
        if (i.macro->val[0] == 0 && i.macro->val[1] == 0) {                    \
          i.macro->val[0] = i.min;                                             \
          i.macro->val[1] = i.max;                                             \
        }                                                                      \
        i.macro->val[0] = CLAMP(i.macro->val[0], i.min, i.max);                \
        i.macro->val[1] = CLAMP(i.macro->val[1], i.min, i.max);                \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  if (ImGui::IsItemHovered()) {                                                \
    switch (i.macro->open & 6) {                                               \
    case 0:                                                                    \
      ImGui::SetTooltip(_("Macro type: Sequence"));                            \
      break;                                                                   \
    case 2:                                                                    \
      ImGui::SetTooltip(_("Macro type: ADSR"));                                \
      break;                                                                   \
    case 4:                                                                    \
      ImGui::SetTooltip(_("Macro type: LFO"));                                 \
      break;                                                                   \
    default:                                                                   \
      ImGui::SetTooltip(_("Macro type: What's going on here?"));               \
      break;                                                                   \
    }                                                                          \
  }                                                                            \
  if (i.macro->open & 6) {                                                     \
    i.macro->len = 16;                                                         \
  }

#define BUTTON_TO_SET_PROPS(_x)                                                \
  ImGui::Button(ICON_FA_ELLIPSIS_H "##IMacroSet");                             \
  if (ImGui::IsItemHovered()) {                                                \
    ImGui::SetTooltip(_("Delay/Step Length"));                                 \
  }                                                                            \
  if (ImGui::BeginPopupContextItem("IMacroSetP",                               \
                                   ImGuiPopupFlags_MouseButtonLeft)) {         \
    if (ImGui::InputScalar(_("Step Length (ticks)##IMacroSpeed"),              \
                           ImGuiDataType_U8, &_x.macro->speed, &_ONE,          \
                           &_THREE)) {                                         \
      if (_x.macro->speed < 1) _x.macro->speed = 1;                            \
    }                                                                          \
    if (ImGui::InputScalar(_("Delay##IMacroDelay"), ImGuiDataType_U8,          \
                           &_x.macro->delay, &_ONE, &_THREE)) {                \
    }                                                                          \
    ImGui::EndPopup();                                                         \
  }

#define BUTTON_TO_SET_RELEASE(buttonType)                                      \
  if (buttonType(ICON_FA_BOLT "##IMacroRelMode")) {                            \
    i.macro->open ^= 8;                                                        \
  }                                                                            \
  if (ImGui::IsItemHovered()) {                                                \
    if (i.macro->open & 8) {                                                   \
      ImGui::SetTooltip(_("Release mode: Active (jump to release pos)"));      \
    } else {                                                                   \
      ImGui::SetTooltip(_("Release mode: Passive (delayed release)"));         \
    }                                                                          \
  }
