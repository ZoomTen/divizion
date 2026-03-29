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

struct EventManager {
  static std::map<uint32_t, Gui*> instances; // Map WindowID -> Gui Instance
  static void Register(uint32_t id, Gui* g);
  static void Unregister(uint32_t id);
  static void PollAll();
};

std::map<uint32_t, Gui*> EventManager::instances;

Gui::Gui(Vst::AEffect *e)
{
  this->effect=e;
}

Gui::~Gui()
{
  if (!this->init) return;

  uint32_t id = SDL_GetWindowID(this->window);
  EventManager::Unregister(id);

  ImGui::SetCurrentContext(this->ctx);
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext(this->ctx);
  this->ctx = nullptr;

  SDL_DestroyRenderer(this->renderer);
  SDL_DestroyWindow(this->window);

  if (this->myWindow)
  {
    DestroyWindow(this->myWindow);
    this->myWindow = nullptr;
  }
  this->init = false;
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
  EventManager::Register(SDL_GetWindowID(this->window), this);

  this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_SOFTWARE);
  if (!this->renderer)
  {
    logE("load renderer error: %s", SDL_GetError());
    return this->init;
  }

  this->ctx = ImGui::CreateContext();
  ImGui::SetCurrentContext(this->ctx);
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
  if (!this->ctx) return;

  EventManager::PollAll();

  ImGui::SetCurrentContext(this->ctx);

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

void Gui::HandleEvent(SDL_Event* ev) {
    ImGui::SetCurrentContext(this->ctx);
    ImGui_ImplSDL2_ProcessEvent(ev);
}

void Gui::RenderGui()
{
  ImGui::SetCurrentContext(this->ctx);

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
    if (ImGui::MenuItem("About")){ this->showAbout = true; }
    ImGui::EndMenuBar();
  }

  ImGui::BeginChild("Contents", ImVec2(0, 0), true);

  if (ImGui::BeginTabBar("MainTabBar", ImGuiTabBarFlags_FittingPolicyScroll|ImGuiTabBarFlags_TabListPopupButton))
  {
    if (ImGui::BeginTabItem("Square 1"))
    {
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Square 2"))
    {
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Wave"))
    {
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Noise"))
    {
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }

  ImGui::EndChild();

  if (this->showAbout) {
    // Pass the pointer to the boolean so the 'X' button can close it
    if (ImGui::Begin("Divizion", &this->showAbout, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Divizion 0.0.1");
        ImGui::Text("powered by Furnace 0.6.8.3");
    }
    ImGui::End();
  }

  ImGui::End();
}


void EventManager::Register(uint32_t id, Gui* g) { instances[id] = g; }
void EventManager::Unregister(uint32_t id) { instances.erase(id); }
void EventManager::PollAll() {
  SDL_Event ev;
  while (SDL_PollEvent(&ev)) {
    uint32_t id = 0;
    if (ev.type >= SDL_WINDOWEVENT && ev.type <= SDL_SYSWMEVENT) id = ev.window.windowID;
    if (ev.type >= SDL_KEYDOWN && ev.type <= SDL_KEYUP) id = ev.key.windowID;
    if (ev.type >= SDL_MOUSEMOTION && ev.type <= SDL_MOUSEWHEEL) id = ev.button.windowID;
    if (instances.count(id)) {
      instances[id]->HandleEvent(&ev);}}
}