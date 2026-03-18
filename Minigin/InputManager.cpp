#include "InputManager.h"
#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>

namespace dae
{
	//  Constructor – pre-create all four controller slots
	InputManager::InputManager()
	{
		for (unsigned int i = 0; i < MaxControllers; ++i)
			m_Controllers.emplace_back(std::make_unique<Controller>(i));
	}

	//  Bind / Unbind – Keyboard
	void InputManager::BindKeyboardCommand(SDL_Keycode key, KeyState state,
		std::unique_ptr<Command> command)
	{
		UnbindKeyboardCommand(key, state);
		m_KeyboardBindings.push_back({ key, state, std::move(command) });
	}

	void InputManager::UnbindKeyboardCommand(SDL_Keycode key, KeyState state)
	{
		m_KeyboardBindings.erase(
			std::remove_if(m_KeyboardBindings.begin(), m_KeyboardBindings.end(),
				[key, state](const KeyboardBinding& b)
				{ return b.key == key && b.state == state; }),
			m_KeyboardBindings.end());
	}

	//  Bind / Unbind – Controller
	void InputManager::BindControllerCommand(unsigned int controllerIndex,
		ControllerButton button, KeyState state,
		std::unique_ptr<Command> command)
	{
		if (controllerIndex >= MaxControllers) return;
		UnbindControllerCommand(controllerIndex, button, state);
		m_ControllerBindings.push_back({ controllerIndex, button, state, std::move(command) });
	}

	void InputManager::UnbindControllerCommand(unsigned int controllerIndex,
		ControllerButton button, KeyState state)
	{
		m_ControllerBindings.erase(
			std::remove_if(m_ControllerBindings.begin(), m_ControllerBindings.end(),
				[controllerIndex, button, state](const ControllerBinding& b)
				{
					return b.controllerIndex == controllerIndex
						&& b.button == button
						&& b.state == state;
				}),
			m_ControllerBindings.end());
	}

	//  ProcessInput
	bool InputManager::ProcessInput()
	{
		// 1. Update controller state (must happen before querying IsDown etc.)
		for (auto& controller : m_Controllers)
			controller->Update();

		// 2. Clear per-frame keyboard transition lists
		m_KeysDown.clear();
		m_KeysUp.clear();

		// 3. Poll SDL events
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			ImGui_ImplSDL3_ProcessEvent(&e);

			if (e.type == SDL_EVENT_QUIT)
				return false;

			if (e.type == SDL_EVENT_KEY_DOWN && !e.key.repeat)
				m_KeysDown.push_back(e.key.key);

			if (e.type == SDL_EVENT_KEY_UP)
				m_KeysUp.push_back(e.key.key);
		}

		// 4. Execute keyboard commands
		const bool* keyboardState = SDL_GetKeyboardState(nullptr);

		for (auto& binding : m_KeyboardBindings)
		{
			bool shouldExecute = false;

			switch (binding.state)
			{
			case KeyState::Down:
				shouldExecute = std::find(m_KeysDown.begin(), m_KeysDown.end(),
					binding.key) != m_KeysDown.end();
				break;

			case KeyState::Up:
				shouldExecute = std::find(m_KeysUp.begin(), m_KeysUp.end(),
					binding.key) != m_KeysUp.end();
				break;

			case KeyState::Pressed:
			{
				SDL_Scancode sc = SDL_GetScancodeFromKey(binding.key, nullptr);
				shouldExecute = (sc != SDL_SCANCODE_UNKNOWN) && keyboardState[sc];
				break;
			}
			}

			if (shouldExecute)
				binding.command->Execute();
		}

		// 5. Execute controller commands
		for (auto& binding : m_ControllerBindings)
		{
			const auto& controller = m_Controllers[binding.controllerIndex];
			if (!controller->IsConnected()) continue;

			bool shouldExecute = false;

			switch (binding.state)
			{
			case KeyState::Down:    shouldExecute = controller->IsDown(binding.button);    break;
			case KeyState::Up:      shouldExecute = controller->IsUp(binding.button);      break;
			case KeyState::Pressed: shouldExecute = controller->IsPressed(binding.button); break;
			}

			if (shouldExecute)
				binding.command->Execute();
		}

		return true;
	}
}
