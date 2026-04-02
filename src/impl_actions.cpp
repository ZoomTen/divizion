#include "impl_actions.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"
#include "src/engine/instrument.h"
#include "src/engine/sample.h"
#include "src/engine/wavetable.h"

void drawInfo(int index, DivEngine *e, DivInstrument *i);
void drawInfo(DivWavetable *i);
void drawInfo(DivSample *i);
void drawInvalidPage(void);

DivizionActionsImpl::DivizionActionsImpl(DivEngine *e) { this->e = e; }

void DivizionActionsImpl::drawInstrumentInfo(ActiveItemType type, int index)
{
	DivInstrument *ins;
	DivWavetable *wav;
	DivSample *sam;

#define _GET(x, y) (y < 0) ? nullptr : (y < x.size()) ? x[y] : nullptr;
	switch (type)
	{
		case INSTRUMENT:
			ins = _GET(this->e->song.ins, index);
			if (ins)
				drawInfo(index, e, ins);
			else
				drawInvalidPage();
			break;
		case WAVETABLE:
			wav = _GET(this->e->song.wave, index);
			if (wav)
				drawInfo(wav);
			else
				drawInvalidPage();
			break;
		case SAMPLE:
			sam = _GET(this->e->song.sample, index);
			if (sam)
				drawInfo(sam);
			else
				drawInvalidPage();
			break;
		default:
			drawInvalidPage();
			break;
	}
#undef _GET
}

void DivizionActionsImpl::actAdd(ActiveItemType type) {}
void DivizionActionsImpl::actDuplicate(ActiveItemType type, int index) {}
void DivizionActionsImpl::actLoad(ActiveItemType type) {}
void DivizionActionsImpl::actSave(ActiveItemType type, int index) {}
void DivizionActionsImpl::actMoveUp(ActiveItemType type, int index) {}
void DivizionActionsImpl::actMoveDown(ActiveItemType type, int index) {}
void DivizionActionsImpl::actDelete(ActiveItemType type, int index) {}

vector<DivInstrument *> DivizionActionsImpl::getInstrumentList()
{
	return this->e->song.ins;
}

vector<DivWavetable *> DivizionActionsImpl::getWavetables()
{
	return this->e->song.wave;
}

vector<DivSample *> DivizionActionsImpl::getSamples()
{
	return this->e->song.sample;
}

#include "IconsFontAwesome4.h"
#include "furIcons.h"

const char *insTypes[DIV_INS_MAX + 1][3] = {
    {"SN76489/Sega PSG", ICON_FA_BAR_CHART, ICON_FUR_INS_STD},
    {"FM (OPN)", ICON_FA_AREA_CHART, ICON_FUR_INS_FM},
    {"Game Boy", ICON_FA_GAMEPAD, ICON_FUR_INS_GB},
    {"C64", ICON_FA_KEYBOARD_O, ICON_FUR_INS_C64},
    {_N("Generic Sample"), ICON_FA_VOLUME_UP, ICON_FUR_INS_AMIGA},
    {"PC Engine", ICON_FA_ID_BADGE, ICON_FUR_INS_PCE},
    {"AY-3-8910/SSG", ICON_FA_BAR_CHART, ICON_FUR_INS_AY},
    {"AY8930", ICON_FA_BAR_CHART, ICON_FUR_INS_AY8930},
    {"TIA", ICON_FA_BAR_CHART, ICON_FUR_INS_TIA},
    {"SAA1099", ICON_FA_BAR_CHART, ICON_FUR_INS_SAA1099},
    {"VIC", ICON_FA_BAR_CHART, ICON_FUR_INS_VIC},
    {"PET", ICON_FA_SQUARE, ICON_FUR_INS_PET},
    {"VRC6", ICON_FA_BAR_CHART, ICON_FUR_INS_VRC6},
    {"FM (OPLL)", ICON_FA_AREA_CHART, ICON_FUR_INS_OPLL},
    {"FM (OPL)", ICON_FA_AREA_CHART, ICON_FUR_INS_OPL},
    {"FDS", ICON_FA_FLOPPY_O, ICON_FUR_INS_FDS},
    {"Virtual Boy", ICON_FA_BINOCULARS, ICON_FUR_INS_VBOY},
    {"Namco 163", ICON_FA_CALCULATOR, ICON_FUR_INS_N163},
    {"Konami SCC/Bubble System WSG", ICON_FA_CALCULATOR, ICON_FUR_INS_SCC},
    {"FM (OPZ)", ICON_FA_AREA_CHART, ICON_FUR_INS_OPZ},
    {"POKEY", ICON_FA_BAR_CHART, ICON_FUR_INS_POKEY},
    {"Beeper", ICON_FA_SQUARE, ICON_FUR_INS_BEEPER},
    {"WonderSwan", ICON_FA_GAMEPAD, ICON_FUR_INS_SWAN},
    {"Atari Lynx", ICON_FA_BAR_CHART, ICON_FUR_INS_MIKEY},
    {"VERA", ICON_FA_KEYBOARD_O, ICON_FUR_INS_VERA},
    {"X1-010", ICON_FA_BAR_CHART, ICON_FUR_INS_X1_010},
    {_("VRC6 (saw)"), ICON_FA_BAR_CHART, ICON_FUR_INS_VRC6_SAW},
    {"ES5506", ICON_FA_VOLUME_UP, ICON_FUR_INS_ES5506},
    {"MultiPCM/OPL4 PCM", ICON_FA_VOLUME_UP, ICON_FUR_INS_MULTIPCM},
    {"SNES", ICON_FA_VOLUME_UP, ICON_FUR_INS_SNES},
    {"Sound Unit", ICON_FA_MICROCHIP, ICON_FUR_INS_SU},
    {"Namco WSG", ICON_FA_PIE_CHART, ICON_FUR_INS_NAMCO},
    {_N("OPL (drums)"), ICON_FA_COFFEE, ICON_FUR_INS_OPL_DRUMS},
    {"FM (OPM)", ICON_FA_AREA_CHART, ICON_FUR_INS_OPM},
    {"NES", ICON_FA_GAMEPAD, ICON_FUR_INS_NES},
    {"MSM6258", ICON_FA_VOLUME_UP, ICON_FUR_INS_MSM6258},
    {"MSM6295", ICON_FA_VOLUME_UP, ICON_FUR_INS_MSM6295},
    {"ADPCM-A", ICON_FA_VOLUME_UP, ICON_FUR_INS_ADPCMA},
    {"ADPCM-B", ICON_FA_VOLUME_UP, ICON_FUR_INS_ADPCMB},
    {"SegaPCM", ICON_FA_VOLUME_UP, ICON_FUR_INS_SEGAPCM},
    {"QSound", ICON_FA_VOLUME_UP, ICON_FUR_INS_QSOUND},
    {"YMZ280B", ICON_FA_VOLUME_UP, ICON_FUR_INS_YMZ280B},
    {"RF5C68", ICON_FA_VOLUME_UP, ICON_FUR_INS_RF5C68},
    {"MSM5232", ICON_FA_BAR_CHART, ICON_FUR_INS_MSM5232},
    {"T6W28", ICON_FA_BAR_CHART, ICON_FUR_INS_T6W28},
    {"K007232", ICON_FA_BAR_CHART, ICON_FUR_INS_K007232},
    {"GA20", ICON_FA_BAR_CHART, ICON_FUR_INS_GA20},
    {"Pokémon Mini/QuadTone", ICON_FA_BAR_CHART, ICON_FUR_INS_POKEMINI},
    {"SM8521", ICON_FA_GAMEPAD, ICON_FUR_INS_SM8521},
    {"PV-1000", ICON_FA_GAMEPAD, ICON_FUR_INS_PV1000},
    {"K053260", ICON_FA_BAR_CHART, ICON_FUR_INS_K053260},
    {"SCSP", ICON_FA_QUESTION, ICON_FUR_INS_SCSP},
    {"TED", ICON_FA_BAR_CHART, ICON_FUR_INS_TED},
    {"C140", ICON_FA_VOLUME_UP, ICON_FUR_INS_C140},
    {"C219", ICON_FA_VOLUME_UP, ICON_FUR_INS_C219},
    {"FM (ESFM)", ICON_FA_AREA_CHART, ICON_FUR_INS_ESFM},
    {_N("PowerNoise (noise)"), ICON_FUR_NOISE, ICON_FUR_INS_POWERNOISE},
    {_N("PowerNoise (slope)"), ICON_FUR_SAW, ICON_FUR_INS_POWERNOISE_SAW},
    {"Dave", ICON_FA_BAR_CHART, ICON_FUR_INS_DAVE},
    {"Nintendo DS", ICON_FA_BAR_CHART, ICON_FUR_INS_NDS},
    {"GBA DMA", ICON_FA_GAMEPAD, ICON_FUR_INS_GBA_DMA},
    {"GBA MinMod", ICON_FA_VOLUME_UP, ICON_FUR_INS_GBA_MINMOD},
    {"Bifurcator", ICON_FA_LINE_CHART, ICON_FUR_INS_BIFURCATOR},
    {"SID2", ICON_FA_KEYBOARD_O, ICON_FUR_INS_SID2},
    {"Watara Supervision", ICON_FA_GAMEPAD, ICON_FUR_INS_SUPERVISION},
    {"NEC μPD1771C", ICON_FA_BAR_CHART, ICON_FUR_INS_UPD1771C},
    {"SID3", ICON_FA_KEYBOARD_O, ICON_FUR_INS_SID3},
    {NULL, ICON_FA_QUESTION, ICON_FA_QUESTION}};

struct FurnaceGUIMacroDesc
{
	DivInstrument *ins;
	DivInstrumentMacro *macro;
	int min, max;
	float height;
	const char *displayName;
	const char **bitfieldBits;
	const char *modeName;
	ImVec4 color;
	bool isBitfield, blockMode, bit30;
	String (*hoverFunc)(int, float, void *);
	void *hoverFuncUser;
	bool isArp;
	bool isPitch;

	FurnaceGUIMacroDesc(const char *name, DivInstrumentMacro *m, int macroMin,
	                    int macroMax, float macroHeight,
	                    ImVec4 col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
	                    bool block = false, const char *mName = NULL,
	                    String (*hf)(int, float, void *) = NULL,
	                    bool bitfield = false, const char **bfVal = NULL,
	                    bool bit30Special = false, void *hfu = NULL,
	                    bool isArp = false, bool isPitch = false)
	    : ins(NULL), macro(m), height(macroHeight), displayName(name),
	      bitfieldBits(bfVal), modeName(mName), color(col),
	      isBitfield(bitfield), blockMode(block), bit30(bit30Special),
	      hoverFunc(hf), hoverFuncUser(hfu), isArp(isArp), isPitch(isPitch)
	{
		// MSVC -> hell
		this->min = macroMin;
		this->max = macroMax;
	}
};

#define _(x) x

const int _ZERO = 0;
const int _ONE = 1;
const int _THREE = 3;
const int _FOUR = 4;
const int _SEVEN = 7;
const int _EIGHT = 8;
const int _TEN = 10;
const int _FIFTEEN = 15;
const int _SIXTEEN = 16;
const int _TWENTY_FOUR = 24;
const int _THIRTY_ONE = 31;
const int _SIXTY_FOUR = 64;
const int _ONE_HUNDRED = 100;
const int _ONE_HUNDRED_TWENTY_SEVEN = 127;
const int _ONE_HUNDRED_SEVENTY_NINE = 179;
const int _TWO_HUNDRED_FIFTY_FIVE = 255;
const int _FIVE_HUNDRED_ELEVEN = 511;
const int _TWO_THOUSAND_FORTY_SEVEN = 2047;
const int _FOUR_THOUSAND_NINETY_FIVE = 4095;
const int _SIXTY_FIVE_THOUSAND_FIVE_HUNDRED_THIRTY_FIVE = 65535;
const int _MINUS_TWENTY_FOUR = -24;
const int _MINUS_ONE_HUNDRED_TWENTY_SEVEN = -127;
const int _MINUS_ONE_HUNDRED_TWENTY_EIGHT = -128;
int wheelX, wheelY, dragSourceX, dragSourceXFine, dragSourceY, dragDestinationX,
    dragDestinationXFine, dragDestinationY, oldBeat, oldBar;
bool isCtrlWheelModifierHeld()
{
	return ImGui::IsKeyDown(ImGuiMod_Ctrl) || ImGui::IsKeyDown(ImGuiMod_Super);
}
#define CW_ADDITION(T)                                                         \
	if (p_min != NULL && p_max != NULL)                                        \
	{                                                                          \
		if (*((T *) p_min) > *((T *) p_max))                                   \
		{                                                                      \
			if (wheelY < 0)                                                    \
			{                                                                  \
				if ((*((T *) p_data) - wheelY) > *((T *) p_min))               \
				{                                                              \
					*((T *) p_data) = *((T *) p_min);                          \
				}                                                              \
				else                                                           \
				{                                                              \
					*((T *) p_data) -= wheelY;                                 \
				}                                                              \
			}                                                                  \
			else                                                               \
			{                                                                  \
				if ((*((T *) p_data) - wheelY) < *((T *) p_max))               \
				{                                                              \
					*((T *) p_data) = *((T *) p_max);                          \
				}                                                              \
				else                                                           \
				{                                                              \
					*((T *) p_data) -= wheelY;                                 \
				}                                                              \
			}                                                                  \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			if (wheelY > 0)                                                    \
			{                                                                  \
				if ((*((T *) p_data) + wheelY) > *((T *) p_max))               \
				{                                                              \
					*((T *) p_data) = *((T *) p_max);                          \
				}                                                              \
				else                                                           \
				{                                                              \
					*((T *) p_data) += wheelY;                                 \
				}                                                              \
			}                                                                  \
			else                                                               \
			{                                                                  \
				if ((*((T *) p_data) + wheelY) < *((T *) p_min))               \
				{                                                              \
					*((T *) p_data) = *((T *) p_min);                          \
				}                                                              \
				else                                                           \
				{                                                              \
					*((T *) p_data) += wheelY;                                 \
				}                                                              \
			}                                                                  \
		}                                                                      \
	}
#define ctrlWheeling (isCtrlWheelModifierHeld() && wheelY != 0)

bool CWSliderScalar(const char *label, ImGuiDataType data_type, void *p_data,
                    const void *p_min, const void *p_max,
                    const char *format = NULL, ImGuiSliderFlags flags = 0)
{
	flags ^= ImGuiSliderFlags_AlwaysClamp;
	if (ImGui::SliderScalar(label, data_type, p_data, p_min, p_max, format,
	                        flags))
	{
		return true;
	}
	if (ImGui::IsItemHovered() && ctrlWheeling)
	{
		switch (data_type)
		{
			case ImGuiDataType_U8:
				CW_ADDITION(unsigned char);
				break;
			case ImGuiDataType_S8:
				CW_ADDITION(signed char);
				break;
			case ImGuiDataType_U16:
				CW_ADDITION(unsigned short);
				break;
			case ImGuiDataType_S16:
				CW_ADDITION(short);
				break;
			case ImGuiDataType_U32:
				CW_ADDITION(unsigned int);
				break;
			case ImGuiDataType_S32:
				CW_ADDITION(int);
				break;
			case ImGuiDataType_Float:
				CW_ADDITION(float);
				break;
			case ImGuiDataType_Double:
				CW_ADDITION(double);
				break;
		}
		return true;
	}
	return false;
}

#define PARAMETER
#define dpiScale 1.0

void addAALine(ImDrawList *dl, const ImVec2 &p1, const ImVec2 &p2,
               const ImU32 color, float thickness = 1.0f)
{
	ImVec2 pt[2];
	pt[0] = p1;
	pt[1] = p2;
	dl->AddPolyline(pt, 2, color, ImDrawFlags_None, thickness);
}
void drawGBEnv(unsigned char vol, unsigned char len, unsigned char sLen,
               bool dir, const ImVec2 &size)
{
	ImDrawList *dl = ImGui::GetWindowDrawList();
	ImGuiWindow *window = ImGui::GetCurrentWindow();

	ImVec2 minArea = window->DC.CursorPos;
	ImVec2 maxArea = ImVec2(minArea.x + size.x, minArea.y + size.y);
	ImRect rect = ImRect(minArea, maxArea);
	ImGuiStyle &style = ImGui::GetStyle();
	// ImU32 color=ImGui::GetColorU32(uiColors[GUI_COLOR_FM_ENVELOPE]);
	ImU32 color = ImGui::GetColorU32(0xff888800);
	// ImU32
	// colorS=ImGui::GetColorU32(uiColors[GUI_COLOR_FM_ENVELOPE_SUS_GUIDE]); //
	// Sustain horiz/vert line color
	ImGui::ItemSize(size, style.FramePadding.y);
	if (ImGui::ItemAdd(rect, ImGui::GetID("gbEnv")))
	{
		ImGui::RenderFrame(rect.Min, rect.Max,
		                   ImGui::GetColorU32(ImGuiCol_FrameBg), true,
		                   style.FrameRounding);

		float volY = 1.0 - ((float) vol / 15.0);
		float lenPos = (sLen > 62) ? 1.0 : ((float) sLen / 384.0);
		float envEndPoint =
		    ((float) len / 7.0) * ((float) (dir ? (15 - vol) : vol) / 15.0);

		ImVec2 pos1 = ImLerp(rect.Min, rect.Max, ImVec2(0.0, volY));
		ImVec2 pos2;
		if (dir)
		{
			if (len > 0)
			{
				if (lenPos < envEndPoint)
				{
					pos2 = ImLerp(
					    rect.Min, rect.Max,
					    ImVec2(lenPos, volY * (1.0 - (lenPos / envEndPoint))));
				}
				else
				{
					pos2 = ImLerp(rect.Min, rect.Max, ImVec2(envEndPoint, 0.0));
				}
			}
			else
			{
				pos2 = ImLerp(rect.Min, rect.Max, ImVec2(lenPos, volY));
			}
		}
		else
		{
			if (len > 0)
			{
				if (lenPos < envEndPoint)
				{
					pos2 = ImLerp(
					    rect.Min, rect.Max,
					    ImVec2(lenPos,
					           volY + (1.0 - volY) * (lenPos / envEndPoint)));
				}
				else
				{
					pos2 = ImLerp(rect.Min, rect.Max, ImVec2(envEndPoint, 1.0));
				}
			}
			else
			{
				pos2 = ImLerp(rect.Min, rect.Max, ImVec2(lenPos, volY));
			}
		}
		ImVec2 pos3 =
		    ImLerp(rect.Min, rect.Max,
		           ImVec2(lenPos, (len > 0 || sLen < 63)
		                              ? ((dir && sLen > 62) ? 0.0 : 1.0)
		                              : volY));

		addAALine(dl, pos1, pos2, color);
		if (lenPos >= envEndPoint && sLen < 63 && dir)
		{
			pos3 = ImLerp(rect.Min, rect.Max, ImVec2(lenPos, 0.0));
			addAALine(dl, pos2, pos3, color);
			ImVec2 pos4 = ImLerp(rect.Min, rect.Max, ImVec2(lenPos, 1.0));
			addAALine(dl, pos3, pos4, color);
		}
		else
		{
			addAALine(dl, pos2, pos3, color);
		}
	}
}

bool LocalizedComboGetter(void *data, int idx, const char **out_text)
{
	const char *const *items = (const char *const *) data;
	if (out_text)
		*out_text = _(items[idx]);
	return true;
}
#define MARK_MODIFIED
const char *gbHWSeqCmdTypes[6] = {_N("Envelope"), _N("Sweep"),
                                  _N("Wait"),     _N("Wait for Release"),
                                  _N("Loop"),     _N("Loop until Release")};
bool CWSliderInt(const char *label, int *v, int v_min, int v_max,
                 const char *format = NULL, ImGuiSliderFlags flags = 0)
{
	return CWSliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format,
	                      flags);
}

void drawInfo(int index, DivEngine *e, DivInstrument *ins)
{
	if (ImGui::BeginTable("InsProp", 3))
	{
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
		int insType = ins->type;
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		bool warnType = true;
		for (DivInstrumentType i : e->getPossibleInsTypes())
		{
			if (i == insType)
			{
				warnType = false;
			}
		}
		// pushWarningColor(warnType, warnType && failedNoteOn);
		if (ImGui::BeginCombo("##Type", (insType >= DIV_INS_MAX)
		                                    ? _("Unknown")
		                                    : _(insTypes[insType][0])))
		{
			std::vector<DivInstrumentType> insTypeList;
			for (int i = 0; insTypes[i][0]; i++)
			{
				insTypeList.push_back((DivInstrumentType) i);
			}
			for (DivInstrumentType i : insTypeList)
			{
				if (ImGui::Selectable(insTypes[i][0], insType == i))
				{
					ins->type = i;

					// reset macro zoom
					memset(ins->temp.vZoom, -1, sizeof(ins->temp.vZoom));
				}
			}
			ImGui::EndCombo();
		}
		else if (warnType)
		{
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip(_("none of the currently present chips are "
				                    "able to play this instrument type!"));
			}
		}
		// popWarningColor();
		ImGui::EndTable();
	}
#define P(x) x
#define rightClickable                                                         \
	if (ImGui::IsItemClicked(ImGuiMouseButton_Right))                          \
		ImGui::SetKeyboardFocusHere(-1);
	if (ImGui::BeginTabBar("insEditTab"))
	{
		std::vector<FurnaceGUIMacroDesc> macroList;

		if (ins->type == DIV_INS_GB)
		{
			if (ImGui::BeginTabItem("Game Boy"))
			{
				P(ImGui::Checkbox(_("Use software envelope"),
				                  &ins->gb.softEnv));
				P(ImGui::Checkbox(_("Initialize envelope on every note"),
				                  &ins->gb.alwaysInit));
				P(ImGui::Checkbox(_("Double wave length (GBA only)"),
				                  &ins->gb.doubleWave));

				ImGui::BeginDisabled(ins->gb.softEnv);
				if (ImGui::BeginTable("GBParams", 2))
				{
					ImGui::TableSetupColumn(
					    "c0", ImGuiTableColumnFlags_WidthStretch, 0.6f);
					ImGui::TableSetupColumn(
					    "c1", ImGuiTableColumnFlags_WidthStretch, 0.4f);

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					if (ImGui::BeginTable("GBParamsI", 2))
					{
						ImGui::TableSetupColumn(
						    "ci0", ImGuiTableColumnFlags_WidthFixed);
						ImGui::TableSetupColumn(
						    "ci1", ImGuiTableColumnFlags_WidthStretch);

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text(_("Volume"));
						ImGui::TableNextColumn();
						ImGui::SetNextItemWidth(
						    ImGui::GetContentRegionAvail().x);
						P(CWSliderScalar("##GBVolume", ImGuiDataType_U8,
						                 &ins->gb.envVol, &_ZERO, &_FIFTEEN));
						rightClickable

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text(_("Length"));
						ImGui::TableNextColumn();
						ImGui::SetNextItemWidth(
						    ImGui::GetContentRegionAvail().x);
						P(CWSliderScalar("##GBEnvLen", ImGuiDataType_U8,
						                 &ins->gb.envLen, &_ZERO, &_SEVEN));
						rightClickable

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text(_("Sound Length"));
						ImGui::TableNextColumn();
						ImGui::SetNextItemWidth(
						    ImGui::GetContentRegionAvail().x);
						P(CWSliderScalar(
						    "##GBSoundLen", ImGuiDataType_U8, &ins->gb.soundLen,
						    &_ZERO, &_SIXTY_FOUR,
						    ins->gb.soundLen > 63 ? _("Infinity") : "%d"));
						rightClickable

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text(_("Direction"));
						ImGui::TableNextColumn();
						bool goesUp = ins->gb.envDir;
						if (ImGui::RadioButton(_("Up"), goesUp))
						{
							PARAMETER
							goesUp = true;
							ins->gb.envDir = goesUp;
						}
						ImGui::SameLine();
						if (ImGui::RadioButton(_("Down"), !goesUp))
						{
							PARAMETER
							goesUp = false;
							ins->gb.envDir = goesUp;
						}

						ImGui::EndTable();
					}

					ImGui::TableNextColumn();
					drawGBEnv(ins->gb.envVol, ins->gb.envLen, ins->gb.soundLen,
					          ins->gb.envDir,
					          ImVec2(ImGui::GetContentRegionAvail().x,
					                 100.0f * dpiScale));

					ImGui::EndTable();
				}

				if (ImGui::BeginChild("HWSeq", ImGui::GetContentRegionAvail(),
				                      true, ImGuiWindowFlags_MenuBar))
				{
					ImGui::BeginMenuBar();
					ImGui::Text(_("Hardware Sequence"));
					ImGui::EndMenuBar();

					if (ins->gb.hwSeqLen > 0)
						if (ImGui::BeginTable("HWSeqList", 3))
						{
							ImGui::TableSetupColumn(
							    "c0", ImGuiTableColumnFlags_WidthFixed);
							ImGui::TableSetupColumn(
							    "c1", ImGuiTableColumnFlags_WidthStretch);
							ImGui::TableSetupColumn(
							    "c2", ImGuiTableColumnFlags_WidthFixed);
							int curFrame = 0;
							ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
							ImGui::TableNextColumn();
							ImGui::Text(_("Tick"));
							ImGui::TableNextColumn();
							ImGui::Text(_("Command"));
							ImGui::TableNextColumn();
							ImGui::Text(_("Move/Remove"));
							for (int i = 0; i < ins->gb.hwSeqLen; i++)
							{
								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								ImGui::Text("%d (#%d)", curFrame, i);
								ImGui::TableNextColumn();
								ImGui::PushID(i);
								if (ins->gb.hwSeq[i].cmd >=
								    DivInstrumentGB::DIV_GB_HWCMD_MAX)
								{
									ins->gb.hwSeq[i].cmd = 0;
								}
								int cmd = ins->gb.hwSeq[i].cmd;
								ImGui::SetNextItemWidth(
								    ImGui::GetContentRegionAvail().x);
								if (ImGui::Combo(
								        "##HWSeqCmd", &cmd,
								        LocalizedComboGetter, gbHWSeqCmdTypes,
								        DivInstrumentGB::DIV_GB_HWCMD_MAX))
								{
									if (ins->gb.hwSeq[i].cmd != cmd)
									{
										ins->gb.hwSeq[i].cmd = cmd;
										ins->gb.hwSeq[i].data = 0;
									}
								}
								bool somethingChanged = false;
								switch (ins->gb.hwSeq[i].cmd)
								{
									case DivInstrumentGB::DIV_GB_HWCMD_ENVELOPE:
									{
										int hwsVol =
										    (ins->gb.hwSeq[i].data & 0xf0) >> 4;
										bool hwsDir = ins->gb.hwSeq[i].data & 8;
										int hwsLen = ins->gb.hwSeq[i].data & 7;
										int hwsSoundLen =
										    ins->gb.hwSeq[i].data >> 8;

										if (CWSliderInt(_("Volume"), &hwsVol, 0,
										                15))
										{
											somethingChanged = true;
										}
										if (CWSliderInt(_("Env Length"),
										                &hwsLen, 0, 7))
										{
											somethingChanged = true;
										}
										if (CWSliderInt(_("Sound Length"),
										                &hwsSoundLen, 0, 64,
										                hwsSoundLen > 63
										                    ? _("Infinity")
										                    : "%d"))
										{
											somethingChanged = true;
										}
										if (ImGui::RadioButton(_("Up"), hwsDir))
										{
											PARAMETER
											hwsDir = true;
											somethingChanged = true;
										}
										ImGui::SameLine();
										if (ImGui::RadioButton(_("Down"),
										                       !hwsDir))
										{
											PARAMETER
											hwsDir = false;
											somethingChanged = true;
										}

										if (somethingChanged)
										{
											ins->gb.hwSeq[i].data =
											    (hwsLen & 7) |
											    (hwsDir ? 8 : 0) |
											    (hwsVol << 4) |
											    (hwsSoundLen << 8);
											PARAMETER;
										}
										break;
									}
									case DivInstrumentGB::DIV_GB_HWCMD_SWEEP:
									{
										int hwsShift =
										    ins->gb.hwSeq[i].data & 7;
										int hwsSpeed =
										    (ins->gb.hwSeq[i].data & 0x70) >> 4;
										bool hwsDir = ins->gb.hwSeq[i].data & 8;

										if (CWSliderInt(_("Shift"), &hwsShift,
										                0, 7))
										{
											somethingChanged = true;
										}
										if (CWSliderInt(_("Speed"), &hwsSpeed,
										                0, 7))
										{
											somethingChanged = true;
										}

										if (ImGui::RadioButton(_("Up"),
										                       !hwsDir))
										{
											PARAMETER
											hwsDir = false;
											somethingChanged = true;
										}
										ImGui::SameLine();
										if (ImGui::RadioButton(_("Down"),
										                       hwsDir))
										{
											PARAMETER
											hwsDir = true;
											somethingChanged = true;
										}

										if (somethingChanged)
										{
											ins->gb.hwSeq[i].data =
											    (hwsShift & 7) |
											    (hwsDir ? 8 : 0) |
											    (hwsSpeed << 4);
											PARAMETER;
										}
										break;
									}
									case DivInstrumentGB::DIV_GB_HWCMD_WAIT:
									{
										int len = ins->gb.hwSeq[i].data + 1;
										curFrame += ins->gb.hwSeq[i].data + 1;

										if (ImGui::InputInt(_("Ticks"), &len, 1,
										                    4))
										{
											if (len < 1)
												len = 1;
											if (len > 255)
												len = 256;
											somethingChanged = true;
										}

										if (somethingChanged)
										{
											ins->gb.hwSeq[i].data = len - 1;
											PARAMETER;
										}
										break;
									}
									case DivInstrumentGB::DIV_GB_HWCMD_WAIT_REL:
										curFrame++;
										break;
									case DivInstrumentGB::DIV_GB_HWCMD_LOOP:
									case DivInstrumentGB::DIV_GB_HWCMD_LOOP_REL:
									{
										int pos = ins->gb.hwSeq[i].data;

										if (ImGui::InputInt(_("Position"), &pos,
										                    1, 1))
										{
											if (pos < 0)
												pos = 0;
											if (pos > (ins->gb.hwSeqLen - 1))
												pos = (ins->gb.hwSeqLen - 1);
											somethingChanged = true;
										}

										if (somethingChanged)
										{
											ins->gb.hwSeq[i].data = pos;
											PARAMETER;
										}
										break;
									}
									default:
										break;
								}
								ImGui::PopID();
								ImGui::TableNextColumn();
								ImGui::PushID(i + 512);
								if (ImGui::Button(ICON_FA_CHEVRON_UP
								                  "##HWCmdUp"))
								{
									if (i > 0)
									{
										e->lockEngine([ins, i]() {
											ins->gb.hwSeq[i - 1].cmd ^=
											    ins->gb.hwSeq[i].cmd;
											ins->gb.hwSeq[i].cmd ^=
											    ins->gb.hwSeq[i - 1].cmd;
											ins->gb.hwSeq[i - 1].cmd ^=
											    ins->gb.hwSeq[i].cmd;

											ins->gb.hwSeq[i - 1].data ^=
											    ins->gb.hwSeq[i].data;
											ins->gb.hwSeq[i].data ^=
											    ins->gb.hwSeq[i - 1].data;
											ins->gb.hwSeq[i - 1].data ^=
											    ins->gb.hwSeq[i].data;
										});
									}
									MARK_MODIFIED;
								}
								ImGui::SameLine();
								if (ImGui::Button(ICON_FA_CHEVRON_DOWN
								                  "##HWCmdDown"))
								{
									if (i < ins->gb.hwSeqLen - 1)
									{
										e->lockEngine([ins, i]() {
											ins->gb.hwSeq[i + 1].cmd ^=
											    ins->gb.hwSeq[i].cmd;
											ins->gb.hwSeq[i].cmd ^=
											    ins->gb.hwSeq[i + 1].cmd;
											ins->gb.hwSeq[i + 1].cmd ^=
											    ins->gb.hwSeq[i].cmd;

											ins->gb.hwSeq[i + 1].data ^=
											    ins->gb.hwSeq[i].data;
											ins->gb.hwSeq[i].data ^=
											    ins->gb.hwSeq[i + 1].data;
											ins->gb.hwSeq[i + 1].data ^=
											    ins->gb.hwSeq[i].data;
										});
									}
									MARK_MODIFIED;
								}
								ImGui::SameLine();
								// pushDestColor();
								if (ImGui::Button(ICON_FA_TIMES "##HWCmdDel"))
								{
									for (int j = i; j < ins->gb.hwSeqLen - 1;
									     j++)
									{
										ins->gb.hwSeq[j].cmd =
										    ins->gb.hwSeq[j + 1].cmd;
										ins->gb.hwSeq[j].data =
										    ins->gb.hwSeq[j + 1].data;
									}
									ins->gb.hwSeqLen--;
								}
								// popDestColor();
								ImGui::PopID();
							}
							ImGui::EndTable();
						}

					if (ImGui::Button(ICON_FA_PLUS "##HWCmdAdd"))
					{
						if (ins->gb.hwSeqLen < 255)
						{
							ins->gb.hwSeq[ins->gb.hwSeqLen].cmd = 0;
							ins->gb.hwSeq[ins->gb.hwSeqLen].data = 0;
							ins->gb.hwSeqLen++;
						}
					}
				}
				ImGui::EndChild();
				ImGui::EndDisabled();
				ImGui::EndTabItem();
			}
		}
	}
}
void drawInfo(DivWavetable *i) {}
void drawInfo(DivSample *i) {}
void drawInvalidPage(void) { ImGui::Text("<- select something..."); }
