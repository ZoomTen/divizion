#include "gui.hpp"
#include "IconsFontAwesome4.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_sw.hpp"
#include "src/engine/instrument.h"
#include "src/engine/sample.h"
#include "src/engine/wavetable.h"
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_log.h>
#include <cstdio>
#include <unordered_map>

#include "file_dialog.hpp"

std::unordered_map<uint32_t, Gui*> Gui::windows;

static void drawInsList(Gui* self);
static void drawAboutWindow(bool* showFlag);
static void renderWindow(Gui* self);

extern void renderListItem(Gui* self, size_t index, DivInstrument* item);
extern void renderListItem(Gui* self, size_t index, DivWavetable* item);
extern void renderListItem(Gui* self, size_t index, DivSample* item);
extern unsigned char ___src_FontAwesome_otf[];
extern unsigned char ___src_icons_ttf[];
extern unsigned int ___src_FontAwesome_otf_len;
extern unsigned int ___src_icons_ttf_len;

#define ICON_MIN_FUR 0xe0f0
#define ICON_MAX_FUR 0xe165
static const ImWchar fontRangeIcon[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
static const ImWchar fontRangeFurIcon[] = { ICON_MIN_FUR, ICON_MAX_FUR, 0 };

Gui::Gui(SDL_Window* w, DivizionActions* act)
{
  this->w = w;
  this->act = act;

  this->wid = SDL_GetWindowID(this->w);
  windows[this->wid] = this;

  this->c = ImGui::CreateContext();
  ImGui::SetCurrentContext(this->c);

  if (!ImGui_ImplSW_Init(this->w)) {
    // try again
    if (!SDL_GetWindowSurface(this->w)) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "can't init UI surface: %s",
                   SDL_GetError());
    }
    if (!ImGui_ImplSW_Init(this->w)) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "can't init UI backend: %s",
                   SDL_GetError());
    }
  }

  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = { 500, 500 };
  io.Fonts->AddFontDefault();
  ImFontConfig config;
  config.MergeMode = true;
  config.PixelSnapH = true;
  config.OversampleH = 1;
  config.OversampleV = 1;
  config.FontDataOwnedByAtlas = false;
  io.IniFilename = NULL;

  io.Fonts->AddFontFromMemoryTTF(
    ___src_FontAwesome_otf, ___src_FontAwesome_otf_len,
    12.0f * io.DisplayFramebufferScale.y, &config, fontRangeIcon);
  io.Fonts->AddFontFromMemoryTTF(___src_icons_ttf, ___src_icons_ttf_len,
                                 12.0f * io.DisplayFramebufferScale.y, &config,
                                 fontRangeFurIcon);

  this->isActive = true; // immediately mark GUI as ready
}

Gui::~Gui()
{
  if (!this->c) return;

  windows.erase(this->wid);
  ImGui::SetCurrentContext(this->c);
  ImGui_ImplSW_Shutdown();
  ImGui::DestroyContext(this->c);
}

void Gui::RenderGui()
{
  if (!this->isActive) return;

  ImGui::SetCurrentContext(this->c);

  ImGui_ImplSW_NewFrame();
  ImGui::NewFrame();

  // make it "full screen"
  auto io = ImGui::GetIO();
  ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  renderWindow(this);
  ImGui::Render();
  ImGui_ImplSW_RenderDrawData(ImGui::GetDrawData());
  SDL_UpdateWindowSurface(this->w);
}

void renderWindow(Gui* self)
{
  ImGui::Begin("Root", NULL,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar
                 | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                 | ImGuiWindowFlags_NoBringToFrontOnFocus);
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Load from project...")) {
        self->isActive = false;
        std::string prjFile = getFileName(false);
        self->isActive = true;
        if (prjFile != "") {
          if (self->act) self->act->loadPrjFile(prjFile);
        }
      }
      if (ImGui::MenuItem("Save to project...")) {
        self->isActive = false;
        std::string prjFile = getFileName(true);
        self->isActive = true;
        if (prjFile != "") {
          if (self->act) self->act->savePrjFile(prjFile);
        }
      }
      ImGui::EndMenu();
    }
    if (ImGui::MenuItem("UI Debug")) {
      self->showDebug = true;
    }
    if (ImGui::MenuItem("Panic")) {
      if (self->act) self->act->panic();
    }
    if (ImGui::MenuItem("About")) {
      self->showAbout = true;
    }
    ImGui::EndMenuBar();
  }
  ImGui::BeginChild("Contents", ImVec2(0, 0));
  if (ImGui::BeginTabBar("AssetsTabBar")) {
    if (ImGui::BeginTabItem("Instruments")) {
      drawInsList(self);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Chips")) {
      if (self->act) self->act->drawChipInfo();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Channel Map")) {
      if (self->act) self->act->drawChanInfo();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Registers")) {
      if (self->act) self->act->drawRegView();
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  ImGui::EndChild();
  if (self->showAbout) drawAboutWindow(&self->showAbout);
  if (self->showDebug) ImGui::ShowMetricsWindow(&self->showDebug);

  if (ImGui::BeginPopupModal("Error", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text(_("%s"), self->act->guiErrorMessage.c_str());
    if (ImGui::Button(_("OK"))) {
      self->act->guiErrorMessage = "";
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  if (self->act)
    if (!self->act->guiErrorMessage.empty()) {
      ImGui::OpenPopup("Error");
    }

  ImGui::End();
}

void drawAboutWindow(bool* showFlag)
{
  if (ImGui::Begin("About", showFlag,
                   ImGuiWindowFlags_AlwaysAutoResize
                     | ImGuiWindowFlags_Modal)) {
    ImGui::Text("Divizion 0.0.1");
    ImGui::Text("powered by DivEngine (Furnace) 0.6.8.3");
    ImGui::Separator();
    ImGui::Text("https://github.com/zoomten/divizion");
    ImGui::Text("GPL v2+");
    ImGui::Text("(c) 2023-2026 Zumi");
    ImGui::Text("Furnace (c) 2021-2026 tildearrow and contributors");
  }
  ImGui::End();
}

void drawInsList(Gui* self)
{
  if (!ImGui::BeginChild("Assets")) return;

  ImVec2 listBox = ImGui::GetContentRegionAvail();
  listBox.y = listBox.y - 28;

  if (!ImGui::BeginChild("FullContainer", listBox)) return;
  if (ImGui::BeginTabBar("AssetsTabBar",
                         ImGuiTabBarFlags_FittingPolicyScroll)) {
    if (ImGui::BeginTabItem("Inst")) {
      self->currentlyViewingType = INSTRUMENT;
      if (self->act) {
        ImGui::BeginChild("Instr list");
        auto list = self->act->getInstrumentList();
        if (list.size() < 1)
        {
          ImGui::Text("no instruments, go to File -> Load from project to load some");
        }
        else
        {
          for (size_t i = 0; i < list.size(); i++) {
            ImGui::PushID(i);
            renderListItem(self, i, list[i]);
            ImGui::PopID();
          }
        }
        ImGui::EndChild();
      } else ImGui::Text("getInstrumentList missing");

      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Wave")) {
      self->currentlyViewingType = WAVETABLE;
      if (self->act) {
        ImGui::BeginChild("Instr list");
        auto list = self->act->getWavetables();
        if (list.size() < 1)
        {
          ImGui::Text("no wavetables");
        }
        else
        {
          for (size_t i = 0; i < list.size(); i++) {
            ImGui::PushID(i);
            renderListItem(self, i, list[i]);
            ImGui::PopID();
          }
        }
        ImGui::EndChild();
      } else ImGui::Text("getWavetables missing");
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Samp")) {
      self->currentlyViewingType = SAMPLE;
      if (self->act) {
        ImGui::BeginChild("Instr list");
        auto list = self->act->getSamples();
        if (list.size() < 1)
        {
          ImGui::Text("no samples");
        }
        else
        {
          for (size_t i = 0; i < list.size(); i++) {
            ImGui::PushID(i);
            renderListItem(self, i, list[i]);
            ImGui::PopID();
          }
        }
        ImGui::EndChild();
      } else ImGui::Text("getWavetables missing");
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  ImGui::EndChild();
  ImGui::EndChild();
}
