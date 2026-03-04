#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include "InputManager.h"

bool dae::InputManager::ProcessInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		// Forward all events to ImGui first
		ImGui_ImplSDL3_ProcessEvent(&e);

		if (e.type == SDL_EVENT_QUIT)
			return false;

		if (e.type == SDL_EVENT_KEY_DOWN)
		{
		}
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
		}
	}
	return true;
}
