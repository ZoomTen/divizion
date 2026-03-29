#include "gui.hpp"
#include "SDL.h"
#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "src/ta-log.h"
#include "src/vst.hpp"
#include <windef.h>
#include <windows.h>

Gui::Gui(Vst::AEffect *e)
{
  this->effect=e;
}

Gui::~Gui()
{
  if (!this->init) return;
  
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
  RECT r;
  HWND parent = (HWND)p;
  this->init = false;
  GetClientRect(parent, &r);

  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) < 0)
  {
    logE("sdl init error: %s", SDL_GetError());
    return this->init;
  }

  logV("win size: %d x %d", r.right-r.left,r.bottom-r.top);
  this->myWindow = CreateWindowExA(0,
    "STATIC", "", WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|SS_NOTIFY,
    0, 0, windowSize.Width(), windowSize.Height(),
    parent, nullptr, nullptr, nullptr);
  
  if (this->myWindow == nullptr)
  {
    logE("init own window error");
    return this->init;
  }

  // window provided by  host
  this->window = SDL_CreateWindowFrom(this->myWindow);
  if (!this->window)
  {
    logE("open window error: %s", SDL_GetError());
    return this->init;
  }

  this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_SOFTWARE);
  if (!this->renderer)
  {
    logE("load renderer error: %s", SDL_GetError());
    return this->init;
  }

  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForSDLRenderer(this->window, this->renderer);
  ImGui_ImplSDLRenderer2_Init(this->renderer);
  ImGui::GetIO().IniFilename = nullptr; // disable layout saving

  this->init = true;
  logV("GUI inited");
  return this->init;
}

void Gui::idle()
{
  if (!this->init) return;

  MSG msg;
  while (PeekMessage(&msg, this->myWindow, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
  }
  
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
                  |ImGuiWindowFlags_NoMove
  );
  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("New", "Ctrl+N")) { 
          /* Handle New */ 
      }
      ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Edit", NULL, false, false)){}
    if (ImGui::MenuItem("About", NULL, false, false)){}
    ImGui::EndMenuBar();
  }
  ImGui::BeginChild("Contents", ImVec2(0, 0), true);
  ImGui::EndChild();
  ImGui::End();
}