#pragma once
#include "imgui.h"

void addAALine(ImDrawList* dl, const ImVec2& p1, const ImVec2& p2,
               const ImU32 color, float thickness = 1.0f);

void drawGBEnv(unsigned char vol, unsigned char len, unsigned char sLen,
               bool dir, const ImVec2& size);

bool LocalizedComboGetter(void* data, int idx, const char** out_text);

#define P(x) x
#define rightClickable                                                         \
  if (ImGui::IsItemClicked(ImGuiMouseButton_Right))                            \
    ImGui::SetKeyboardFocusHere(-1);