#include "IconsFontAwesome4.h"
#include "imgui.h"
#include "src/impl_actions.hpp"

static void drawSystemChannelInfo(const DivSysDef* whichDef);
static void drawSystemChannelInfoText(const DivSysDef* whichDef);

void DivizionActionsImpl::drawChipInfo()
{
  if (ImGui::BeginTable("SystemList", 3)) {
    ImGui::TableSetupColumn("c1", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("c2", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("c3", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
    ImGui::TableNextColumn();
    ImGui::TableNextColumn();
    ImGui::Text(_("Name"));
    ImGui::TableNextColumn();
    ImGui::Text(_("Actions"));
    for (unsigned char i = 0; i < e->song.systemLen; i++) {
      ImGui::PushID(i);
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      if (ImGui::Button(ICON_FA_ARROWS)) {
      }
      if (ImGui::BeginDragDropSource()) {
        sysToMove = i;
        ImGui::SetDragDropPayload("FUR_SYS", NULL, 0, ImGuiCond_Once);
        ImGui::Button(ICON_FA_ARROWS "##SysDrag");
        ImGui::EndDragDropSource();
      } else if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(_("(drag to swap chips)"));
      }
      if (ImGui::BeginDragDropTarget()) {
        const ImGuiPayload* dragItem = ImGui::AcceptDragDropPayload("FUR_SYS");
        if (dragItem != NULL) {
          if (dragItem->IsDataType("FUR_SYS")) {
            if (sysToMove != i && sysToMove >= 0) {
              e->swapSystem(sysToMove, i, preserveChanPos);
              
            }
            sysToMove = -1;
          }
        }
        ImGui::EndDragDropTarget();
      }
      ImGui::TableNextColumn();
      ImGui::Text(
        "%d. %s##_SYSM%d",
        i + 1,
        e->getSystemName(e->song.system[i]),
        i
      );
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary)) {
        if (e->song.system[i] != DIV_SYSTEM_NULL) {
          const DivSysDef* sysDef = e->getSystemDef(e->song.system[i]);
          if (ImGui::BeginTooltip()) {
            // ImGui::Dummy(ImVec2(MIN(scrW, 400.0f), 0.0f));
            // ImGui::PushTextWrapPos(
            //   MIN(scrW, 400.0f)); // arbitrary constant
            ImGui::TextWrapped("%s", sysDef->description);
            ImGui::Separator();
            drawSystemChannelInfoText(sysDef);
            drawSystemChannelInfo(sysDef);
            ImGui::EndTooltip();
          }
        }
      }
      ImGui::PopID();
    }
    ImGui::EndTable();
  }
}

void drawSystemChannelInfo(const DivSysDef* whichDef) {
  const ImVec2 p=ImGui::GetCursorScreenPos();
  float scaler=5.0f;
  float x=p.x, y=p.y;
  float tooltipWidth=400.0f;//MIN(scrW,400.0f);
  for (int i=0; i<whichDef->channels; i++) {
    // dl->AddRectFilled(ImVec2(x,y),ImVec2(x+1.5f*scaler,y+1.0f*scaler),ImGui::GetColorU32(uiColors[whichDef->chanTypes[i]+GUI_COLOR_CHANNEL_FM]),scaler);
    x+=2.0f*scaler;
     if ((x+1.5f*scaler)>tooltipWidth+p.x) {
      x=p.x;
      y+=1.5f*scaler;
    }
  }
  ImGui::Dummy(ImVec2(0,(y-p.y)+1.5f*scaler));
}

enum FurnaceGUIChanTypes {
  // the first five match DivChanTypes, do not change order!

  CHANNEL_TYPE_FM,
  CHANNEL_TYPE_PULSE,
  CHANNEL_TYPE_NOISE,
  CHANNEL_TYPE_WAVETABLE,
  CHANNEL_TYPE_SAMPLE, 

  CHANNEL_TYPE_SQUARE,
  CHANNEL_TYPE_TRIANGLE,
  CHANNEL_TYPE_SAW,
  CHANNEL_TYPE_OPERATOR,
  CHANNEL_TYPE_DRUMS,
  CHANNEL_TYPE_SLOPE,
  CHANNEL_TYPE_WAVE,
  CHANNEL_TYPE_PSG,
  CHANNEL_TYPE_OTHER,

  CHANNEL_TYPE_MAX
};

unsigned char chanNamesHierarchy[CHANNEL_TYPE_MAX+1]={
  CHANNEL_TYPE_FM,
  CHANNEL_TYPE_OPERATOR,
  CHANNEL_TYPE_SQUARE,
  CHANNEL_TYPE_PULSE,
  CHANNEL_TYPE_PSG,
  CHANNEL_TYPE_WAVETABLE,
  CHANNEL_TYPE_TRIANGLE,
  CHANNEL_TYPE_SAW,
  CHANNEL_TYPE_NOISE,
  CHANNEL_TYPE_SLOPE,
  CHANNEL_TYPE_WAVE,
  CHANNEL_TYPE_DRUMS,
  CHANNEL_TYPE_SAMPLE,
  CHANNEL_TYPE_OTHER,
  CHANNEL_TYPE_MAX
};


const char* chanNames[CHANNEL_TYPE_MAX+1]={
  _N("FM"),
  _N("Pulse"),
  _N("Noise"),
  _N("Wavetable"),
  _N("Sample"),
  // the "freaks":
  _N("Square"),
  _N("Triangle"), // NES
  _N("Saw"), // VRC6
  _N("Ext. Operator"), 
  _N("Drums"),
  _N("Slope"), // PowerNoise
  _N("Wave"), // not wavetable (VERA, 5E01)
  _N("PSG"),

  _N("Channel"), // if neither
  _N("Channels") // in case this makes l10n easier
};
void drawSystemChannelInfoText(const DivSysDef* whichDef) {
  String info="";
  // same order as chanNames
  // helper: FM|PU|NO|WA|SA | SQ|TR|SW|OP|DR|SL|WV|CH
  unsigned char chanCount[CHANNEL_TYPE_MAX];
  memset(chanCount,0,CHANNEL_TYPE_MAX);
  // count channel types
  for (int i=0; i<whichDef->channels; i++) {
    switch (whichDef->chanInsType[i][0]) {
      case DIV_INS_STD: // square
      case DIV_INS_BEEPER:
      case DIV_INS_TED:
      case DIV_INS_VIC:
      case DIV_INS_T6W28:
      case DIV_INS_PV1000:
        if (whichDef->id==0xfd) { // dummy
          chanCount[CHANNEL_TYPE_OTHER]++;
          break;
        }
        if (whichDef->id==0x9f) { // zx sfx
          chanCount[CHANNEL_TYPE_PULSE]++;
          break;
        }
        if (whichDef->chanTypes[i]==DIV_CH_NOISE) { // sn/t6w noise
          chanCount[CHANNEL_TYPE_NOISE]++;
        } else { // DIV_CH_PULSE, any sqr chan
          chanCount[CHANNEL_TYPE_SQUARE]++;
        }
        break;
      case DIV_INS_NES:
        if (whichDef->chanTypes[i]==DIV_CH_WAVE) {
          chanCount[whichDef->id==0xf1?CHANNEL_TYPE_WAVE:CHANNEL_TYPE_TRIANGLE]++; // triangle, wave for 5E01
        } else {
          chanCount[whichDef->chanTypes[i]]++;
        }
        break;
      case DIV_INS_AY:
      case DIV_INS_AY8930:
        chanCount[CHANNEL_TYPE_PSG]++;
        break;
      case DIV_INS_OPL_DRUMS:
      case DIV_INS_OPL:
      case DIV_INS_OPLL:
        if (whichDef->chanTypes[i]==DIV_CH_OP) {
          chanCount[CHANNEL_TYPE_FM]++; // opl3 4op
          break;
        }
        if (whichDef->chanTypes[i]==DIV_CH_NOISE) {
          chanCount[CHANNEL_TYPE_DRUMS]++; // drums
        } else {
          chanCount[whichDef->chanTypes[i]]++;
        }
        break;
      case DIV_INS_FM:
        if (whichDef->chanTypes[i]==DIV_CH_OP) {
          chanCount[CHANNEL_TYPE_OPERATOR]++; // ext. ops
        } else if (whichDef->chanTypes[i]==DIV_CH_NOISE) {
          break; // csm timer
        } else {
          chanCount[whichDef->chanTypes[i]]++;
        }
        break;
      case DIV_INS_ADPCMA:
      case DIV_INS_ADPCMB:
        chanCount[CHANNEL_TYPE_SAMPLE]++;
        break;
      case DIV_INS_VRC6_SAW:
        chanCount[CHANNEL_TYPE_SAW]++;
        break;
      case DIV_INS_POWERNOISE_SLOPE:
        chanCount[CHANNEL_TYPE_SLOPE]++;
        break;
      case DIV_INS_QSOUND:
        chanCount[CHANNEL_TYPE_SAMPLE]++;
        break;
      case DIV_INS_NDS:
        if (whichDef->chanTypes[i]!=DIV_CH_PCM) { // the psg chans can also play samples??
          chanCount[CHANNEL_TYPE_SAMPLE]++;
        }
        chanCount[whichDef->chanTypes[i]]++;
        break;
      case DIV_INS_VERA:
        if (whichDef->chanTypes[i]==DIV_CH_PULSE) {
          chanCount[CHANNEL_TYPE_WAVE]++;
        } else { // sample chan
          chanCount[CHANNEL_TYPE_SAMPLE]++;
        }
        break;
      case DIV_INS_DAVE:
        if (whichDef->chanTypes[i]==DIV_CH_WAVE) {
          chanCount[CHANNEL_TYPE_OTHER]++;
        } else {
          chanCount[whichDef->chanTypes[i]]++;
        }
        break;
      case DIV_INS_SWAN:
        if (whichDef->chanTypes[i]!=DIV_CH_WAVE) {
          chanCount[CHANNEL_TYPE_WAVETABLE]++;
        }
        chanCount[whichDef->chanTypes[i]]++;
        break;
      case DIV_INS_SID3:
        if (whichDef->chanTypes[i]!=DIV_CH_WAVE) {
          chanCount[CHANNEL_TYPE_OTHER]++;
        } else {
          chanCount[CHANNEL_TYPE_WAVE]++;
        }
        break;
      case DIV_INS_C64: // uncategorizable (by me)
      case DIV_INS_TIA:
      case DIV_INS_PET:
      case DIV_INS_SU:
      case DIV_INS_POKEY:
      case DIV_INS_MIKEY:
      case DIV_INS_BIFURCATOR:
      case DIV_INS_SID2:
        chanCount[CHANNEL_TYPE_OTHER]++;
        break;
      default:
        chanCount[whichDef->chanTypes[i]]++;
        break;
    }
  }
  // generate string
  for (int j=0; j<CHANNEL_TYPE_MAX; j++) {
    unsigned char i=chanNamesHierarchy[j];
    if (chanCount[i]==0) continue;
    if (info.length()!=0) {
      info+=", ";
    }
    if (i==CHANNEL_TYPE_OTHER) {
      if (chanCount[i]>1) {
        info+=fmt::sprintf("%d %s",chanCount[i],chanNames[CHANNEL_TYPE_OTHER+1]);
      } else {
        info+=fmt::sprintf("%d %s",chanCount[i],chanNames[CHANNEL_TYPE_OTHER]);
      }
      continue;
    }
    info+=fmt::sprintf("%d × %s",chanCount[i],chanNames[i]);
  }
  ImGui::Text("%s",info.c_str());
}
