#include "gui.hpp"
#include "SDL.h"
#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_hints.h"
#include "SDL_log.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "src/vst.hpp"

Gui::Gui(Vst::AEffect *e)
{
  this->effect=e;
}

Gui::~Gui()
{
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(this->renderer);
  SDL_DestroyWindow(this->window);
  SDL_Quit();
}

Vst::ERect windowSize = {
  0, 0,
  500, 500
};

bool Gui::getRect(Vst::ERect **r)
{
  *r = &windowSize;
  return true;
}

bool Gui::open(void *p)
{
  this->init = false;

  if (SDL_VideoInit(nullptr))
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
      "init video error: %s\n", SDL_GetError());
    return this->init;
  }

  // window provided by VST host
  this->window = SDL_CreateWindowFrom(p);
  if (!this->window)
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
      "open window error: %s\n", SDL_GetError());
    return this->init;
  }

  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
  this->renderer = SDL_CreateRenderer(this->window, -1, 0);
  if (!this->renderer)
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
      "load renderer error: %s\n", SDL_GetError());
    return this->init;
  }

  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForSDLRenderer(this->window, this->renderer);
  ImGui_ImplSDLRenderer2_Init(this->renderer);
  ImGui::GetIO().IniFilename = nullptr; // disable layout saving

  this->init = true;
  return this->init;
}

void Gui::idle()
{
  if (!this->init) return;

  while (SDL_PollEvent(&this->event))
    ImGui_ImplSDL2_ProcessEvent(&this->event);

  // setup platform
  ImGui_ImplSDLRenderer2_NewFrame();
  ImGui_ImplSDL2_NewFrame();

  // clear background
  SDL_SetRenderDrawColor(this->renderer, 100, 100, 100, 255);
  SDL_RenderClear(this->renderer);

  // execute imgui
  ImGui::NewFrame();

  this->RenderGui();

  ImGui::Render();

  // to SDL backbuffer
  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
  SDL_RenderPresent(renderer);
}

void Gui::RenderGui()
{
  // make it "full screen"
  auto io = ImGui::GetIO();
  ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
  ImGui::SetNextWindowPos(ImVec2(0, 0));

  ImGui::Begin("Root", NULL,
                  ImGuiWindowFlags_NoTitleBar
                  |ImGuiWindowFlags_MenuBar
                  |ImGuiWindowFlags_NoResize
  );
  if (ImGui::BeginMenuBar()) {
      if (ImGui::MenuItem("File", NULL, false, false)){}
      if (ImGui::MenuItem("Edit", NULL, false, false)){}
      if (ImGui::MenuItem("About", NULL, false, false)){}
      ImGui::EndMenuBar();
  }
  { // Main contents
      ImGui::BeginChild("Contents", ImVec2(0, 0));
      ImGui::EndChild();
  }
  // ImGui::ShowDemoWindow();
  ImGui::End();
}