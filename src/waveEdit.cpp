/**
 * Furnace Tracker - multi-system chiptune tracker
 * Copyright (C) 2021-2025 tildearrow and contributors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "src/impl_actions.hpp"
#define _USE_MATH_DEFINES
#include "plot_nolerp.hpp"
#include "gui_process_drags.hpp"
#include "text_input_widget.hpp"
#include "mml.hpp"
#include <fmt/printf.h>
#include <math.h>
#include <imgui.h>

#define _N(x) x

const char* waveGenBaseShapes[4] = { _N("Sine"), _N("Triangle"), _N("Saw"),
                                     _N("Pulse") };

const char* waveInterpolations[4] = { _N("None"), _N("Linear"), _N("Cosine"),
                                      _N("Cubic") };

typedef double (*WaveFunc)(double a);

double sinus(double x);
double rectSin(double x);
double absSin(double x);
double square(double x);
double rectSquare(double x);
double quartSin(double x);
double squiSin(double x);
double squiAbsSin(double x);
double saw(double x);
double rectSaw(double x);
double absSaw(double x);
double cubSaw(double x);
double rectCubSaw(double x);
double absCubSaw(double x);
double cubSine(double x);
double rectCubSin(double x);
double absCubSin(double x);
double quartCubSin(double x);
double squishCubSin(double x);
double squishAbsCubSin(double x);
double triangle(double x);
double rectTri(double x);
double absTri(double x);
double quartTri(double x);
double squiTri(double x);
double absSquiTri(double x);
double cubTriangle(double x);
double cubRectTri(double x);
double cubAbsTri(double x);
double cubQuartTri(double x);
double cubSquiTri(double x);
double absCubSquiTri(double x);

WaveFunc waveFuncs[] = { sinus,       rectSin,      absSin,
                         quartSin,    squiSin,      squiAbsSin,

                         square,      rectSquare,

                         saw,         rectSaw,      absSaw,

                         cubSaw,      rectCubSaw,   absCubSaw,

                         cubSine,     rectCubSin,   absCubSin,
                         quartCubSin, squishCubSin, squishAbsCubSin,

                         triangle,    rectTri,      absTri,
                         quartTri,    squiTri,      absSquiTri,

                         cubTriangle, cubRectTri,   cubAbsTri,
                         cubQuartTri, cubSquiTri,   absCubSquiTri };

const char* fmWaveforms[] = {
  _N("Sine"),
  _N("Rect. Sine"),
  _N("Abs. Sine"),
  _N("Quart. Sine"),
  _N("Squish. Sine"),
  _N("Abs. Squish. Sine"),

  _N("Square"),
  _N("rectSquare"),

  _N("Saw"),
  _N("Rect. Saw"),
  _N("Abs. Saw"),

  _N("Cubed Saw"),
  _N("Rect. Cubed Saw"),
  _N("Abs. Cubed Saw"),

  _N("Cubed Sine"),
  _N("Rect. Cubed Sine"),
  _N("Abs. Cubed Sine"),
  _N("Quart. Cubed Sine"),
  _N("Squish. Cubed Sine"),
  _N("Squish. Abs. Cub. Sine"),

  _N("Triangle"),
  _N("Rect. Triangle"),
  _N("Abs. Triangle"),
  _N("Quart. Triangle"),
  _N("Squish. Triangle"),
  _N("Abs. Squish. Triangle"),

  _N("Cubed Triangle"),
  _N("Rect. Cubed Triangle"),
  _N("Abs. Cubed Triangle"),
  _N("Quart. Cubed Triangle"),
  _N("Squish. Cubed Triangle"),
  _N("Squish. Abs. Cub. Triangle"),
};

const size_t fmWaveformsLen = sizeof(fmWaveforms) / sizeof(fmWaveforms[0]);

const float multFactors[17] = {
  M_PI,      2 * M_PI,  4 * M_PI,  6 * M_PI,  8 * M_PI,  10 * M_PI,
  12 * M_PI, 14 * M_PI, 16 * M_PI, 18 * M_PI, 20 * M_PI, 22 * M_PI,
  24 * M_PI, 26 * M_PI, 28 * M_PI, 30 * M_PI, 32 * M_PI,
};

void doGenerateWave(DivizionActionsImpl* self)
{
  float finalResult[256];
  if (self->curWave < 0 || self->curWave >= (int)self->e->song.wave.size())
    return;

  DivWavetable* wave = self->e->song.wave[self->curWave];
  memset(finalResult, 0, sizeof(float) * 256);

  if (wave->len < 2) return;

#if 0
  if (waveGenFM) {
    float s0fb0=0;
    float s0fb1=0;
    float s1fb0=0;
    float s1fb1=0;
    float s2fb0=0;
    float s2fb1=0;
    float s3fb0=0;
    float s3fb1=0;

    float s0=0;
    float s1=0;
    float s2=0;
    float s3=1;

    for (int i=0; i<wave->len; i++) {
      float pos=(float)i/(float)wave->len;
      
      s0=waveFuncs[fmWaveform[0]]((pos +
         (waveGenFB[0] ? ((s0fb0 + s0fb1) * pow(2.0f, waveGenFB[0] - 8)) : 0.0f) +
         (waveGenFMCon0[3] ? s3 : 0.0f) +
         (waveGenFMCon0[2] ? s2 : 0.0f) +
         (waveGenFMCon0[1] ? s1 : 0.0f) +
         (waveGenFMCon0[0] ? s0 : 0.0f))* multFactors[waveGenMult[0]]) * waveGenTL[0];
       
      s0fb0=s0fb1;
      s0fb1=s0;
      
      s1=waveFuncs[fmWaveform[1]]((pos +
         (waveGenFB[1] ? ((s1fb0 + s1fb1) * pow(2.0f, waveGenFB[1] - 8)) : 0.0f) +
         (waveGenFMCon1[3] ? s3 : 0.0f) +
         (waveGenFMCon1[2] ? s2 : 0.0f) +
         (waveGenFMCon1[1] ? s1 : 0.0f) +
         (waveGenFMCon1[0] ? s0 : 0.0f))* multFactors[waveGenMult[1]]) * waveGenTL[1];
      
      s1fb0=s1fb1;
      s1fb1=s1;

      s2=waveFuncs[fmWaveform[2]]((pos +
         (waveGenFB[2] ? ((s2fb0 + s2fb1) * pow(2.0f, waveGenFB[2] - 8)) : 0.0f) +
         (waveGenFMCon2[3] ? s3 : 0.0f) +
         (waveGenFMCon2[2] ? s2 : 0.0f) +
         (waveGenFMCon2[1] ? s1 : 0.0f) +
         (waveGenFMCon2[0] ? s0 : 0.0f))* multFactors[waveGenMult[2]]) * waveGenTL[2];
      
      s2fb0=s2fb1;
      s2fb1=s2;

      s3=waveFuncs[fmWaveform[3]]((pos +
         (waveGenFB[3] ? ((s3fb0 + s3fb1) * pow(2.0f, waveGenFB[3] - 8)) : 0.0f) +
         (waveGenFMCon3[3] ? s3 : 0.0f) +
         (waveGenFMCon3[2] ? s2 : 0.0f) +
         (waveGenFMCon3[1] ? s1 : 0.0f) +
         (waveGenFMCon3[0] ? s0 : 0.0f)) * multFactors[waveGenMult[3]])* waveGenTL[3];
      
      s3fb0=s3fb1;
      s3fb1=s3;

      if (waveGenFMCon0[4]) finalResult[i]+=s0;
      if (waveGenFMCon1[4]) finalResult[i]+=s1;
      if (waveGenFMCon2[4]) finalResult[i]+=s2;
      if (waveGenFMCon3[4]) finalResult[i]+=s3;
    }
  } else {
    switch (waveGenBaseShape) {
      case 0: // sine
        for (int i=0; i<wave->len; i++) {
          for (int j=0; j<16; j++) {
            float pos=fmod((waveGenPhase[j]*wave->len)+(i*(j+1)),wave->len);
            float partial=sin((0.5+pos)*2.0*M_PI/(double)wave->len);
            partial=pow(partial,waveGenPower);
            partial*=waveGenAmp[j];
            finalResult[i]+=partial;
          }
        }
        break;
      case 1: // triangle
        for (int i=0; i<wave->len; i++) {
          for (int j=0; j<16; j++) {
            float pos=fmod((waveGenPhase[j]*wave->len)+(i*(j+1)),wave->len);
            float partial=4.0*(0.5-fabs(0.5-(pos/(double)(wave->len-1))))-1.0;
            partial=pow(partial,waveGenPower);
            partial*=waveGenAmp[j];
            finalResult[i]+=partial;
          }
        }
        break;
      case 2: // saw
        for (int i=0; i<wave->len; i++) {
          for (int j=0; j<16; j++) {
            float pos=fmod((waveGenPhase[j]*wave->len)+(i*(j+1)),wave->len);
            float partial=((2*pos)/(double)(wave->len-1))-1.0;
            partial=pow(partial,waveGenPower);
            partial*=waveGenAmp[j];
            finalResult[i]+=partial;
          }
        }
        break;
      case 3: // pulse
        for (int i=0; i<wave->len; i++) {
          for (int j=0; j<16; j++) {
            float pos=fmod((waveGenPhase[j]*wave->len)+(i*(j+1)),wave->len);
            float partial=(pos>=(waveGenDuty*wave->len))?1:-1;
            partial=pow(partial,waveGenPower);
            partial*=waveGenAmp[j];
            finalResult[i]+=partial;
          }
        }
        break;
    }
  }
#endif

  for (int i = self->waveGenInvertPoint * wave->len; i < wave->len; i++) {
    finalResult[i] = -finalResult[i];
  }

  for (int i = 0; i < wave->len; i++) {
    finalResult[i] = (1.0 + finalResult[i]) * 0.5;
    if (finalResult[i] < 0.0f) finalResult[i] = 0.0f;
    if (finalResult[i] > 1.0f) finalResult[i] = 1.0f;
    wave->data[i] = round(finalResult[i] * wave->max);
  }

  self->e->notifyWaveChange(self->curWave);
}

#define CENTER_TEXT(text)                                                      \
  ImGui::SetCursorPosX(                                                        \
    ImGui::GetCursorPosX()                                                     \
    + 0.5 * (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(text).x));

void drawWaveEdit(DivizionActionsImpl* self, DivWavetable* wave)
{
  float wavePreview[257];

  if (ImGui::BeginTable("WEProps", 1)) {
    ImGui::TableSetupColumn("c0", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
#if 0
    ImGui::SetNextItemWidth(80.0f);
    if (ImGui::RadioButton(_("Steps"), self->waveEditStyle == 0)) {
      self->waveEditStyle = 0;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton(_("Lines"), self->waveEditStyle == 1)) {
      self->waveEditStyle = 1;
    }
    ImGui::SameLine();
#endif
    ImGui::Text(_("Width"));
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip(
        _("use a width of:\n- any on Amiga/N163\n- 32 on Game Boy, PC "
          "Engine, SCC, Konami Bubble System, Namco WSG, Virtual Boy and "
          "WonderSwan\n- 64 on FDS\n- 128 on X1-010\n- 256 on SID3\nany "
          "other widths will be scaled during playback."));
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(96.0f);
    if (ImGui::InputInt("##_WTW", &wave->len, 1, 16)) {
      if (wave->len > 256) wave->len = 256;
      if (wave->len < 1) wave->len = 1;
      self->e->notifyWaveChange(self->curWave);
    }
    ImGui::SameLine();
    ImGui::Text(_("Height"));
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip(
        _("use a height of:\n- 16 for Game Boy, WonderSwan, Namco WSG, "
          "Konami Bubble System, X1-010 Envelope shape and N163\n- 32 for PC "
          "Engine\n- 64 for FDS and Virtual Boy\n- 256 for X1-010, SCC and "
          "SID3\nany other heights will be scaled during playback."));
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(96.0f);
    int realMax = wave->max + 1;
    if (ImGui::InputInt("##_WTH", &realMax, 1, 16)) {
      if (realMax > 256) realMax = 256;
      if (realMax < 2) realMax = 2;
      wave->max = realMax - 1;
      self->e->notifyWaveChange(self->curWave);
    }
#if 0
    ImGui::SameLine();
    if (ImGui::Button(waveGenVisible ? (ICON_FA_CHEVRON_RIGHT "##WEWaveGen")
                                     : (ICON_FA_CHEVRON_LEFT "##WEWaveGen"))) {
      waveGenVisible = !waveGenVisible;
    }
#endif
    ImGui::EndTable();
  }

  for (int i = 0; i < wave->len; i++) {
    if (wave->data[i] > wave->max) wave->data[i] = wave->max;
    wavePreview[i] = wave->data[i];
    if (self->waveSigned && !self->waveHex) {
      wavePreview[i] -= (int)((wave->max + 1) / 2);
    }
  }
  if (wave->len > 0) wavePreview[wave->len] = wave->data[wave->len - 1];

  if (ImGui::BeginTable("WEWaveSection", 1)) {
    ImGui::TableSetupColumn("c0", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

    ImVec2 contentRegion =
      ImGui::GetContentRegionAvail(); // wavetable graph size determined here
    contentRegion.y -=
      ImGui::GetFrameHeightWithSpacing() + ImGui::GetStyle().WindowPadding.y;
    if (self->waveEditStyle) {
      PlotNoLerp("##Waveform", wavePreview, wave->len + 1, 0, NULL,
                 (self->waveSigned && !self->waveHex)
                   ? (-(int)((wave->max + 1) / 2))
                   : 0,
                 (self->waveSigned && !self->waveHex) ? ((int)(wave->max / 2))
                                                      : wave->max,
                 contentRegion);
    } else {
      PlotCustom("##Waveform", wavePreview, wave->len, 0, NULL,
                 (self->waveSigned && !self->waveHex)
                   ? (-(int)((wave->max + 1) / 2))
                   : 0,
                 (self->waveSigned && !self->waveHex) ? ((int)(wave->max / 2))
                                                      : wave->max,
                 contentRegion, sizeof(float), ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                 0, NULL, NULL, true);
    }
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
      self->waveDragStart = ImGui::GetItemRectMin();
      self->waveDragAreaSize = contentRegion;
      self->waveDragMin = 0;
      self->waveDragMax = wave->max;
      self->waveDragLen = wave->len;
      self->waveDragActive = true;
      self->waveDragTarget = wave->data;
      processDrags(self, ImGui::GetMousePos().x, ImGui::GetMousePos().y);
      self->e->notifyWaveChange(self->curWave);
    }
    ImGui::PopStyleVar();
    ImGui::EndTable();
  }

  if (ImGui::RadioButton(_("Dec"), !self->waveHex)) {
    self->waveHex = false;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton(_("Hex"), self->waveHex)) {
    self->waveHex = true;
  }
  ImGui::SameLine();
  if (!self->waveHex)
    if (ImGui::Button(
          self->waveSigned ? "±##WaveSign" : "+##WaveSign",
          ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()))) {
      self->waveSigned = !self->waveSigned;
    }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip(_("Signed/Unsigned"));
  }
  ImGui::SameLine();
  ImGui::SetNextItemWidth(
    ImGui::GetContentRegionAvail().x); // wavetable text input size found here
  if (InputText("MMLWave", &self->mmlStringW)) {
    int actualData[256];
    decodeMMLStrW(
      self->mmlStringW, actualData, wave->len,
      (self->waveSigned && !self->waveHex) ? (-((wave->max + 1) / 2)) : 0,
      (self->waveSigned && !self->waveHex) ? (wave->max / 2) : wave->max,
      self->waveHex);
    if (self->waveSigned && !self->waveHex) {
      for (int i = 0; i < wave->len; i++) {
        actualData[i] += (wave->max + 1) / 2;
      }
    }
    memcpy(wave->data, actualData, wave->len * sizeof(int));
  }
  if (!ImGui::IsItemActive()) {
    int actualData[256];
    memcpy(actualData, wave->data, 256 * sizeof(int));
    if (self->waveSigned && !self->waveHex) {
      for (int i = 0; i < wave->len; i++) {
        actualData[i] -= (wave->max + 1) / 2;
      }
    }
    encodeMMLStr(self->mmlStringW, actualData, wave->len, -1, -1, self->waveHex,
                 false);
  }
}

double sinus(double x)
{
  return sin(x);
}
double rectSin(double x)
{
  return sin(x) > 0 ? sin(x) : 0;
}
double absSin(double x)
{
  return fabs(sin(x));
}

double square(double x)
{
  return fmod(x, (2 * M_PI)) >= M_PI ? -1 : 1;
}
double rectSquare(double x)
{
  return square(x) > 0 ? square(x) : 0;
}

double quartSin(double x)
{
  return absSin(x) * rectSquare(2 * x);
}
double squiSin(double x)
{
  return sin(x) >= 0 ? sin(2 * x) : 0;
}
double squiAbsSin(double x)
{
  return fabs(squiSin(x));
}

double saw(double x)
{
  return atan(tan(x / 2)) / (M_PI / 2);
}
double rectSaw(double x)
{
  return saw(x) > 0 ? saw(x) : 0;
}
double absSaw(double x)
{
  return saw(x) < 0 ? saw(x) + 1 : saw(x);
}

double cubSaw(double x)
{
  return pow(saw(x), 3);
}
double rectCubSaw(double x)
{
  return pow(rectSaw(x), 3);
}
double absCubSaw(double x)
{
  return pow(absSaw(x), 3);
}

double cubSine(double x)
{
  return pow(sin(x), 3);
}
double rectCubSin(double x)
{
  return pow(rectSin(x), 3);
}
double absCubSin(double x)
{
  return pow(absSin(x), 3);
}
double quartCubSin(double x)
{
  return pow(quartSin(x), 3);
}
double squishCubSin(double x)
{
  return pow(squiSin(x), 3);
}
double squishAbsCubSin(double x)
{
  return pow(squiAbsSin(x), 3);
}

double triangle(double x)
{
  return asin(sin(x)) / (M_PI / 2);
}
double rectTri(double x)
{
  return triangle(x) > 0 ? triangle(x) : 0;
}
double absTri(double x)
{
  return fabs(triangle(x));
}
double quartTri(double x)
{
  return absTri(x) * rectSquare(2 * x);
}
double squiTri(double x)
{
  return sin(x) >= 0 ? triangle(2 * x) : 0;
}
double absSquiTri(double x)
{
  return fabs(squiTri(x));
}

double cubTriangle(double x)
{
  return pow(triangle(x), 3);
}
double cubRectTri(double x)
{
  return pow(rectTri(x), 3);
}
double cubAbsTri(double x)
{
  return pow(absTri(x), 3);
}
double cubQuartTri(double x)
{
  return pow(quartTri(x), 3);
}
double cubSquiTri(double x)
{
  return pow(squiTri(x), 3);
}
double absCubSquiTri(double x)
{
  return fabs(cubSquiTri(x));
}
