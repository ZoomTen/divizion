#include "gui.hpp"
#include "IconsFontAwesome4.h"
#include "gui.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_sw.hpp"
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_log.h>
#include <cstdio>
#include <unordered_map>

std::unordered_map<uint32_t, Gui *> Gui::windows;

static void drawInsList(Gui *self);
static void drawAboutWindow(bool *showFlag);
static void drawInsMgmt(Gui *self);
static void renderWindow(Gui *self);

int lastAssetType;

Gui::Gui(SDL_Window *w, DivizionActions *act)
{
  this->w = w;
  this->act = act;

  this->wid = SDL_GetWindowID(this->w);
  windows[this->wid] = this;

  this->c = ImGui::CreateContext();
  ImGui::SetCurrentContext(this->c);

  if (!ImGui_ImplSW_Init(this->w))
  {
    // try again
    if (!SDL_GetWindowSurface(this->w))
    {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "can't init UI surface: %s",
                  SDL_GetError());
    }
    if (!ImGui_ImplSW_Init(this->w))
    {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "can't init UI backend: %s",
                  SDL_GetError());
    }
  }

  ImGuiIO &io = ImGui::GetIO();
  io.DisplaySize = {500, 500};
  io.Fonts->AddFontDefault();
  ImFontConfig config;
  config.MergeMode = true;
  config.PixelSnapH = true;
  // io.Fonts->AddFontFromFileTTF("FontAwesome.otf", 0.0f, &config);
}

Gui::~Gui()
{
  if (!this->c)
    return;

  windows.erase(this->wid);
  ImGui::SetCurrentContext(this->c);
  ImGui_ImplSW_Shutdown();
  ImGui::DestroyContext(this->c);
}

void Gui::RenderGui()
{
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

void renderWindow(Gui *self)
{
  ImGui::Begin("Root", NULL,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar |
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);
  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Load instruments...")){ /* TODO */ }
      if (ImGui::MenuItem("Save instruments...")){ /* TODO */ }
      ImGui::EndMenu();
    }
    if (ImGui::MenuItem("UI Debug"))
    {
      self->showDebug = true;
    }
    if (ImGui::MenuItem("About"))
    {
      self->showAbout = true;
    }
    ImGui::EndMenuBar();
  }
  ImGui::BeginChild("Contents", ImVec2(0, 0));
  if (ImGui::BeginTabBar("AssetsTabBar"))
  {
    if (ImGui::BeginTabItem("Instrument Management"))
    {
      drawInsMgmt(self);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Chip Management"))
    {
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  ImGui::EndChild();
  if (self->showAbout)
    drawAboutWindow(&self->showAbout);
  if (self->showDebug)
    ImGui::ShowMetricsWindow(&self->showDebug);
  ImGui::End();
}

void drawInsMgmt(Gui *self)
{
  if (ImGui::BeginTable("Split", 2, ImGuiTableFlags_Resizable))
  {
    ImGui::TableSetupColumn("LeftPane", ImGuiTableColumnFlags_WidthStretch,
                            0.25);
    ImGui::TableSetupColumn("RightPane", ImGuiTableColumnFlags_WidthStretch,
                            0.75);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    drawInsList(self);

    ImGui::TableSetColumnIndex(1);
    ImGui::BeginChild("RightChild");
    if (self->act)
      self->act->drawInstrumentInfo(self->selectedType,
                                    self->selectedIndex);
    ImGui::EndChild();
    ImGui::EndTable();
  }
}

void drawAboutWindow(bool *showFlag)
{
  if (ImGui::Begin("About", showFlag,
                   ImGuiWindowFlags_AlwaysAutoResize |
                       ImGuiWindowFlags_Modal))
  {
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

void drawInsList(Gui *self)
{

  if (!ImGui::BeginChild("Assets"))
    return;

  ImVec2 listBox = ImGui::GetContentRegionAvail();
  listBox.y = listBox.y - 28;

  if (!ImGui::BeginChild("FullContainer", listBox))
    return;
  if (ImGui::BeginTabBar("AssetsTabBar",
                         ImGuiTabBarFlags_FittingPolicyScroll))
  {
    if (ImGui::BeginTabItem("Inst"))
    {
      self->currentlyViewingType = INST;
      if (self->act)
      {
        ImGui::BeginChild("Instr list");
        auto list = self->act->getInstrumentList();
        for (size_t i = 0; i < list.size(); i++)
        {
          ImGui::PushID(i);

          if (ImGui::Selectable(list[i].c_str(),
                                i == self->instSelected))
          {
            self->selectedType = self->currentlyViewingType;
            self->selectedIndex = i;
            self->instSelected = i;
          }
          ImGui::PopID();
        }
        ImGui::EndChild();
      }
      else
        ImGui::Text("getInfoA missing");

      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Wave"))
    {
      self->currentlyViewingType = WAVE;
      for (int i = 0; i < 12; i++)
      {
        ImGui::PushID(i);
        if (ImGui::Selectable("DEF", i == self->waveSelected))
        {
          self->selectedType = self->currentlyViewingType;
          self->selectedIndex = i;
          self->waveSelected = i;
        }
        ImGui::PopID();
      }
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Samp"))
    {
      self->currentlyViewingType = SAMP;
      for (int i = 0; i < 12; i++)
      {
        ImGui::PushID(i);
        if (ImGui::Selectable("GHI", i == self->sampSelected))
        {
          self->selectedType = self->currentlyViewingType;
          self->selectedIndex = i;
          self->sampSelected = i;
        }
        ImGui::PopID();
      }
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  ImGui::EndChild();
  ImGui::Separator();
  { /* Toolbar */
    int currentlySelectedOfViewingType;
    switch (self->currentlyViewingType)
    {
      case INST:
        currentlySelectedOfViewingType = self->instSelected;
        break;
      case WAVE:
        currentlySelectedOfViewingType = self->waveSelected;
        break;
      case SAMP:
        currentlySelectedOfViewingType = self->sampSelected;
        break;
      default:
        break;
    }
    { /* Button: Add item */
      if (ImGui::Button(ICON_FA_PLUS "##InsAdd"))
      {
        // if (settings.unifiedDataView) {
        switch (lastAssetType)
        {
          case 0:
            // doAction(GUI_ACTION_INS_LIST_ADD);
            break;
          case 1:
            // doAction(GUI_ACTION_WAVE_LIST_ADD);
            break;
          case 2:
            // doAction(GUI_ACTION_SAMPLE_LIST_ADD);
            break;
        }
        // } else {
        // doAction(GUI_ACTION_INS_LIST_ADD);
        // }
      }
      if (ImGui::IsItemHovered())
      {
        ImGui::SetTooltip("Add");
      }
      if (ImGui::IsItemClicked())
      {
        if (self->act)
          self->act->actAdd(self->currentlyViewingType);
      }
      if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
      {
        // makeInsTypeList=e->getPossibleInsTypes();
        // displayInsTypeList=true;
        // displayInsTypeListMakeInsSample=-1;
      }
    }
    ImGui::SameLine();
    { /* Button: Clone item */
      if (ImGui::Button(ICON_FA_FILES_O "##InsClone"))
      {
        // if (settings.unifiedDataView) {
        switch (lastAssetType)
        {
          case 0:
            // doAction(GUI_ACTION_INS_LIST_DUPLICATE);
            break;
          case 1:
            // doAction(GUI_ACTION_WAVE_LIST_DUPLICATE);
            break;
          case 2:
            // doAction(GUI_ACTION_SAMPLE_LIST_DUPLICATE);
            break;
        }
        // } else {
        // doAction(GUI_ACTION_INS_LIST_DUPLICATE);
        // }
      }
      if (ImGui::IsItemClicked())
      {
        if (self->act)
          self->act->actDuplicate(
              self->currentlyViewingType,
              currentlySelectedOfViewingType);
      }
      if (ImGui::IsItemHovered())
      {
        ImGui::SetTooltip("Duplicate");
      }
    }
    ImGui::SameLine();
    { /* Button: Load item */
      if (ImGui::Button(ICON_FA_FOLDER_OPEN "##InsLoad"))
      {
        // if (settings.unifiedDataView) {
        switch (lastAssetType)
        {
          case 0:
            // doAction(GUI_ACTION_INS_LIST_OPEN);
            break;
          case 1:
            // doAction(GUI_ACTION_WAVE_LIST_OPEN);
            break;
          case 2:
            // doAction(GUI_ACTION_SAMPLE_LIST_OPEN);
            break;
        }
        // } else {
        // doAction(GUI_ACTION_INS_LIST_OPEN);
        // }
      }
      if (ImGui::IsItemClicked())
      {
        if (self->act)
          self->act->actLoad(self->currentlyViewingType);
      }
      if (ImGui::IsItemHovered())
      {
        ImGui::SetTooltip("Open");
      }
    }
    /*
      if (ImGui::BeginPopupContextItem("InsOpenOpt"))
      {
          // if (settings.unifiedDataView) {
          if (ImGui::MenuItem(_("replace instrument...")))
          {
              // doAction((curIns>=0 &&
              //
      curIns<(int)e->song.ins.size())?GUI_ACTION_INS_LIST_OPEN_REPLACE:GUI_ACTION_INS_LIST_OPEN);
          }
          if (ImGui::MenuItem(_("load instrument from TX81Z")))
          {
              // doAction(GUI_ACTION_TX81Z_REQUEST);
          }

          ImGui::Separator();

          if (ImGui::MenuItem(_("replace wavetable...")))
          {
              // doAction((curWave>=0 &&
              //
      curWave<(int)e->song.wave.size())?GUI_ACTION_WAVE_LIST_OPEN_REPLACE:GUI_ACTION_WAVE_LIST_OPEN);
          }

          ImGui::Separator();

          if (ImGui::MenuItem(_("replace sample...")))
          {
              // doAction((curSample>=0 &&
              //
      curSample<(int)e->song.sample.size())?GUI_ACTION_SAMPLE_LIST_OPEN_REPLACE:GUI_ACTION_SAMPLE_LIST_OPEN);
          }
          if (ImGui::MenuItem(_("import raw sample...")))
          {
              // doAction(GUI_ACTION_SAMPLE_LIST_OPEN_RAW);
          }
          if (ImGui::MenuItem(_("import raw sample (replace)...")))
          {
              // doAction((curSample>=0 &&
              //
      curSample<(int)e->song.sample.size())?GUI_ACTION_SAMPLE_LIST_OPEN_REPLACE_RAW:GUI_ACTION_SAMPLE_LIST_OPEN_RAW);
          }
          // } else {
          //   if (ImGui::MenuItem(_("replace..."))) {
          //     doAction((curIns>=0 &&
          //
      curIns<(int)e->song.ins.size())?GUI_ACTION_INS_LIST_OPEN_REPLACE:GUI_ACTION_INS_LIST_OPEN);
          //   }
          //   ImGui::Separator();
          //   if (ImGui::MenuItem(_("load from TX81Z"))) {
          //     doAction(GUI_ACTION_TX81Z_REQUEST);
          //   }
          // }
          ImGui::EndPopup();
      }
      if (ImGui::IsItemHovered())
      {
          ImGui::SetTooltip(_("Open (insert; right-click to replace)"));
      }
      */
    ImGui::SameLine();
    { /* Button: save item */
      if (ImGui::Button(ICON_FA_FLOPPY_O "##InsSave"))
      {
        // if (settings.unifiedDataView) {
        switch (lastAssetType)
        {
          case 0:
            // doAction(GUI_ACTION_INS_LIST_SAVE);
            break;
          case 1:
            // doAction(GUI_ACTION_WAVE_LIST_SAVE);
            break;
          case 2:
            // doAction(GUI_ACTION_SAMPLE_LIST_SAVE);
            break;
        }
      }
      if (ImGui::IsItemClicked())
      {
        if (self->act)
          self->act->actSave(self->currentlyViewingType,
                                  currentlySelectedOfViewingType);
      }
      if (ImGui::IsItemHovered())
      {
        ImGui::SetTooltip("Save");
      }
      if (ImGui::BeginPopupContextItem("InsSaveFormats",
                                       ImGuiMouseButton_Right))
      {
        // if (settings.unifiedDataView) {
        if (ImGui::MenuItem("save instrument as .dmp..."))
        {
          // doAction(GUI_ACTION_INS_LIST_SAVE_DMP);
        }

        ImGui::Separator();

        if (ImGui::MenuItem("save wavetable as .dmw..."))
        {
          // doAction(GUI_ACTION_WAVE_LIST_SAVE_DMW);
        }
        if (ImGui::MenuItem("save raw wavetable..."))
        {
          // doAction(GUI_ACTION_WAVE_LIST_SAVE_RAW);
        }

        ImGui::Separator();

        if (ImGui::MenuItem("save raw sample..."))
        {
          // doAction(GUI_ACTION_SAMPLE_LIST_SAVE_RAW);
        }

        ImGui::Separator();

        if (ImGui::MenuItem("save all instruments..."))
        {
          // doAction(GUI_ACTION_INS_LIST_SAVE_ALL);
        }
        if (ImGui::MenuItem("save all wavetables..."))
        {
          // doAction(GUI_ACTION_WAVE_LIST_SAVE_ALL);
        }
        if (ImGui::MenuItem("save all samples..."))
        {
          // doAction(GUI_ACTION_SAMPLE_LIST_SAVE_ALL);
        }
        ImGui::EndPopup();
      }
    }
    ImGui::SameLine();
    { /* Button: move item up the list */
      if (ImGui::Button(ICON_FA_ARROW_UP "##InsUp"))
      {
        switch (lastAssetType)
        {
          case 0:
            // doAction(GUI_ACTION_INS_LIST_MOVE_UP);
            break;
          case 1:
            // doAction(GUI_ACTION_WAVE_LIST_MOVE_UP);
            break;
          case 2:
            // doAction(GUI_ACTION_SAMPLE_LIST_MOVE_UP);
            break;
        }
      }
      if (ImGui::IsItemClicked())
      {
        if (self->act)
          self->act->actMoveUp(self->currentlyViewingType,
                                    currentlySelectedOfViewingType);
      }
      if (ImGui::IsItemHovered())
      {
        ImGui::SetTooltip("Move up");
      }
    }
    ImGui::SameLine();
    { /* Button: move item down the list */
      if (ImGui::Button(ICON_FA_ARROW_DOWN "##InsDown"))
      {
        switch (lastAssetType)
        {
          case 0:
            // doAction(GUI_ACTION_INS_LIST_MOVE_DOWN);
            break;
          case 1:
            // doAction(GUI_ACTION_WAVE_LIST_MOVE_DOWN);
            break;
          case 2:
            // doAction(GUI_ACTION_SAMPLE_LIST_MOVE_DOWN);
            break;
        }
      }
      if (ImGui::IsItemClicked())
      {
        if (self->act)
          self->act->actMoveDown(self->currentlyViewingType,
                                      currentlySelectedOfViewingType);
      }
      if (ImGui::IsItemHovered())
      {
        ImGui::SetTooltip("Move down");
      }
      if (lastAssetType == 2)
      {
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_VOLUME_UP "##PreviewSampleL"))
        {
          // doAction(GUI_ACTION_SAMPLE_LIST_PREVIEW);
        }
        if (ImGui::IsItemHovered())
        {
          ImGui::SetTooltip("Preview (right click to stop)");
        }
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
          // doAction(GUI_ACTION_SAMPLE_LIST_STOP_PREVIEW);
        }
      }
    }
    ImGui::SameLine();
    { /* Button: delete item */
      // pushDestColor();
      if (ImGui::Button(ICON_FA_TIMES "##InsDelete"))
      {
        switch (lastAssetType)
        {
          case 0:
            // doAction(GUI_ACTION_INS_LIST_DELETE);
            break;
          case 1:
            // doAction(GUI_ACTION_WAVE_LIST_DELETE);
            break;
          case 2:
            // doAction(GUI_ACTION_SAMPLE_LIST_DELETE);
            break;
        }
      }
      if (ImGui::IsItemClicked())
      {
        if (self->act)
          self->act->actDelete(self->currentlyViewingType,
                                    currentlySelectedOfViewingType);
      }
      // popDestColor();
      if (ImGui::IsItemHovered())
      {
        ImGui::SetTooltip("Delete");
      }
    }
  }
  ImGui::EndChild();
}
