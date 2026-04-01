#include "backends/imgui_impl_sdl2.h"
#include "../gui.hpp"
#include "imgui.h"
#include "../mock/mock.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

int main(void)
{
	SDL_Window *w = nullptr;
	SDL_Event e;
	bool run = false;
	Mock m = Mock();

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "can't init video: %s",
		             SDL_GetError());
		return 1;
	}

	w = SDL_CreateWindow("divizion", 0, 0, 500, 500, 0);
	if (!w)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "can't start window: %s",
		             SDL_GetError());
		return 1;
	}

	Gui g = Gui(w, &m);

	run = true;
	while (run)
	{
		while (SDL_PollEvent(&e))
		{
			uint32_t wid = 0;

			if (e.type == SDL_QUIT)
			{
				run = false;
				continue;
			}

			// get window ID
			if (e.type == SDL_WINDOWEVENT)
				wid = e.window.windowID;
			else if (e.type >= SDL_MOUSEMOTION && e.type <= SDL_MOUSEWHEEL)
				wid = e.button.windowID;
			else if (e.type >= SDL_KEYDOWN && e.type <= SDL_TEXTINPUT)
				wid = e.key.windowID;

			// apply events
			if (Gui::windows.count(wid))
			{
				Gui *active = Gui::windows[wid];
				ImGui::SetCurrentContext(active->c);
				ImGuiIO &io = ImGui::GetIO();
				switch (e.type)
				{
					case SDL_MOUSEMOTION:
						io.AddMousePosEvent((float) e.motion.x,
						                    (float) e.motion.y);
						break;
					case SDL_MOUSEBUTTONDOWN:
					case SDL_MOUSEBUTTONUP:
						io.AddMouseButtonEvent(e.button.button - 1,
						                       e.type == SDL_MOUSEBUTTONDOWN);
						break;
					case SDL_MOUSEWHEEL:
						io.AddMouseWheelEvent(e.wheel.x, e.wheel.y);
						break;
				}
			}
		}

		// render the thing regardless of event
		for (auto &i : Gui::windows)
		{
			Gui *active = i.second;
			active->RenderGui();
		}
	}

	SDL_DestroyWindow(w);
	SDL_Quit();
	return 0;
}