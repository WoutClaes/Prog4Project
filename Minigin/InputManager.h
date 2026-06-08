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

		void BindKeyboardCommand(SDL_Keycode key, KeyState state,
			std::unique_ptr<Command> command);
		void UnbindKeyboardCommand(SDL_Keycode key, KeyState state);

		void BindControllerCommand(unsigned int controllerIndex,
			ControllerButton button, KeyState state,
			std::unique_ptr<Command> command);
		void UnbindControllerCommand(unsigned int controllerIndex,
			ControllerButton button, KeyState state);

		void RemoveAllBindings();

		bool IsControllerConnected(unsigned int index) const;

		bool ProcessInput();

	private:
		friend class Singleton<InputManager>;
		InputManager();

		struct KeyboardBinding
		{
			SDL_Keycode key;
			KeyState state;
			std::unique_ptr<Command> command;
		};
		std::vector<KeyboardBinding> m_KeyboardBindings;

		std::vector<SDL_Keycode> m_KeysDown;
		std::vector<SDL_Keycode> m_KeysUp;

		struct ControllerBinding
		{
			unsigned int controllerIndex;
			ControllerButton button;
			KeyState state;
			std::unique_ptr<Command> command;
		};
		std::vector<ControllerBinding> m_ControllerBindings;
		std::vector<std::unique_ptr<Controller>> m_Controllers;
	};

}
