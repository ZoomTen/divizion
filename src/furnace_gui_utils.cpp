
#include "furnace_gui_utils.hpp"
#include "imgui_internal.h"

void addAALine(ImDrawList* dl, const ImVec2& p1, const ImVec2& p2,
               const ImU32 color, float thickness)
{
  ImVec2 pt[2];
  pt[0] = p1;
  pt[1] = p2;
  dl->Flags &= ~ImDrawListFlags_AntiAliasedLines;
  dl->AddPolyline(pt, 2, color, ImDrawFlags_None, thickness);
  dl->Flags |= ImDrawListFlags_AntiAliasedLines;
}

void drawGBEnv(unsigned char vol, unsigned char len, unsigned char sLen,
               bool dir, const ImVec2& size)
{
  ImDrawList* dl = ImGui::GetWindowDrawList();
  ImGuiWindow* window = ImGui::GetCurrentWindow();

  ImVec2 minArea = window->DC.CursorPos;
  ImVec2 maxArea = ImVec2(minArea.x + size.x, minArea.y + size.y);
  ImRect rect = ImRect(minArea, maxArea);
  ImGuiStyle& style = ImGui::GetStyle();
  ImU32 color = ImGui::GetColorU32(0xffffffff);
  ImGui::ItemSize(size, style.FramePadding.y);
  if (ImGui::ItemAdd(rect, ImGui::GetID("gbEnv"))) {
    ImGui::RenderFrame(rect.Min, rect.Max, ImGui::GetColorU32(ImGuiCol_FrameBg),
                       true, style.FrameRounding);

    float volY = 1.0 - ((float)vol / 15.0);
    float lenPos = (sLen > 62) ? 1.0 : ((float)sLen / 384.0);
    float envEndPoint =
      ((float)len / 7.0) * ((float)(dir ? (15 - vol) : vol) / 15.0);

    ImVec2 pos1 = ImLerp(rect.Min, rect.Max, ImVec2(0.0, volY));
    ImVec2 pos2;
    if (dir) {
      if (len > 0) {
        if (lenPos < envEndPoint) {
          pos2 = ImLerp(rect.Min, rect.Max,
                        ImVec2(lenPos, volY * (1.0 - (lenPos / envEndPoint))));
        } else {
          pos2 = ImLerp(rect.Min, rect.Max, ImVec2(envEndPoint, 0.0));
        }
      } else {
        pos2 = ImLerp(rect.Min, rect.Max, ImVec2(lenPos, volY));
      }
    } else {
      if (len > 0) {
        if (lenPos < envEndPoint) {
          pos2 = ImLerp(
            rect.Min, rect.Max,
            ImVec2(lenPos, volY + (1.0 - volY) * (lenPos / envEndPoint)));
        } else {
          pos2 = ImLerp(rect.Min, rect.Max, ImVec2(envEndPoint, 1.0));
        }
      } else {
        pos2 = ImLerp(rect.Min, rect.Max, ImVec2(lenPos, volY));
      }
    }
    ImVec2 pos3 = ImLerp(rect.Min, rect.Max,
                         ImVec2(lenPos, (len > 0 || sLen < 63)
                                          ? ((dir && sLen > 62) ? 0.0 : 1.0)
                                          : volY));

    addAALine(dl, pos1, pos2, color);
    if (lenPos >= envEndPoint && sLen < 63 && dir) {
      pos3 = ImLerp(rect.Min, rect.Max, ImVec2(lenPos, 0.0));
      addAALine(dl, pos2, pos3, color);
      ImVec2 pos4 = ImLerp(rect.Min, rect.Max, ImVec2(lenPos, 1.0));
      addAALine(dl, pos3, pos4, color);
    } else {
      addAALine(dl, pos2, pos3, color);
    }
  }
}

#define _(x) x
bool LocalizedComboGetter(void* data, int idx, const char** out_text)
{
  const char* const* items = (const char* const*)data;
  if (out_text) *out_text = _(items[idx]);
  return true;
}