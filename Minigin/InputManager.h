#pragma once
#include "Singleton.h"
#include "Commands/Command.h"
#include "Controller.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <cstdint>

#include <SDL3/SDL_keycode.h>

namespace dae
{
	class InputManager final : public Singleton<InputManager>
	{
	public:
		static constexpr unsigned int MaxControllers = 4;

		// Keyboard binding
		// key   : SDL_Keycode  (e.g. SDLK_W, SDLK_A …)
		// state : Down | Up | Pressed
		void BindKeyboardCommand(SDL_Keycode key, KeyState state,
			std::unique_ptr<Command> command);
		void UnbindKeyboardCommand(SDL_Keycode key, KeyState state);

		// Controller binding
		// controllerIndex : 0-3
		void BindControllerCommand(unsigned int controllerIndex,
			ControllerButton button, KeyState state,
			std::unique_ptr<Command> command);
		void UnbindControllerCommand(unsigned int controllerIndex,
			ControllerButton button, KeyState state);

		// Main loop
		bool ProcessInput();

	private:
		friend class Singleton<InputManager>;
		InputManager();

		// Keyboard state
		struct KeyboardBinding
		{
			SDL_Keycode             key;
			KeyState                state;
			std::unique_ptr<Command> command;
		};
		std::vector<KeyboardBinding> m_KeyboardBindings;

		std::vector<SDL_Keycode> m_KeysDown;    // pressed this frame
		std::vector<SDL_Keycode> m_KeysUp;      // released this frame

		// Controller state
		struct ControllerBinding
		{
			unsigned int            controllerIndex;
			ControllerButton        button;
			KeyState                state;
			std::unique_ptr<Command> command;
		};
		std::vector<ControllerBinding>          m_ControllerBindings;
		std::vector<std::unique_ptr<Controller>> m_Controllers;
	};

}
