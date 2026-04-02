#include "IconsFontAwesome4.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "src/impl_actions.hpp"

#define MARK_MODIFIED

int chanToMove;

void DivizionActionsImpl::drawChannelInfo()
{
  if (ImGui::BeginTable("ChannelList", 5)) {
    ImGui::TableSetupColumn("c0", ImGuiTableColumnFlags_WidthFixed, 0.0);
    ImGui::TableSetupColumn("c1", ImGuiTableColumnFlags_WidthFixed, 0.0);
    ImGui::TableSetupColumn("c2", ImGuiTableColumnFlags_WidthFixed, 0.0);
    ImGui::TableSetupColumn("c3", ImGuiTableColumnFlags_WidthStretch, 0.0);
    ImGui::TableSetupColumn("c4", ImGuiTableColumnFlags_WidthFixed, 48.0f);
    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
    ImGui::TableNextColumn();
    ImGui::Text(_("Pat"));
    ImGui::TableNextColumn();
    ImGui::Text(_("Osc"));
    ImGui::TableNextColumn();
    ImGui::Text(_("Swap"));
    ImGui::TableNextColumn();
    ImGui::Text(_("Name"));
    for (int i = 0; i < e->getTotalChannelCount(); i++) {
      ImGui::PushID(i);
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      if (ImGui::Checkbox("##VisiblePat", &this->e->curSubSong->chanShow[i])) {
        MARK_MODIFIED;
      }
      if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(_("Show in pattern"));
      }
      ImGui::TableNextColumn();
      if (ImGui::Checkbox("##VisibleChanOsc",
                          &e->curSubSong->chanShowChanOsc[i])) {
        MARK_MODIFIED;
      }
      if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(_("Show in per-channel oscilloscope"));
      }
      ImGui::TableNextColumn();
      if (ImGui::Button(ICON_FA_ARROW_UP)) {
      }
      if (ImGui::BeginDragDropSource()) {
        chanToMove = i;
        ImGui::SetDragDropPayload("FUR_CHAN", NULL, 0, ImGuiCond_Once);
        ImGui::Button(ICON_FA_ARROWS "##ChanDrag");
        ImGui::EndDragDropSource();
      } else if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(_("%s #%d\n(drag to swap channels)"),
                          e->getSystemName(e->sysOfChan[i]),
                          e->dispatchChanOfChan[i]);
      }
      if (ImGui::BeginDragDropTarget()) {
        const ImGuiPayload* dragItem = ImGui::AcceptDragDropPayload("FUR_CHAN");
        if (dragItem != NULL) {
          if (dragItem->IsDataType("FUR_CHAN")) {
            if (chanToMove != i && chanToMove >= 0) {
              e->swapChannelsP(chanToMove, i);
              MARK_MODIFIED;
            }
            chanToMove = -1;
          }
        }
        ImGui::EndDragDropTarget();
      }
      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
      if (ImGui::InputTextWithHint("##ChanName", e->getChannelName(i),
                                   &e->curSubSong->chanName[i])) {
        MARK_MODIFIED;
      }
      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
      if (ImGui::InputTextWithHint("##ChanShortName", e->getChannelShortName(i),
                                   &e->curSubSong->chanShortName[i])) {
        MARK_MODIFIED;
      }
      ImGui::PopID();
    }
    ImGui::EndTable();
  }
}