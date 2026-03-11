#include "Controller.h"
#include <cstring>

// Platform-specific includes (hidden inside the .cpp)
#if defined(__EMSCRIPTEN__)
#  include <SDL3/SDL.h>
#else
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <Xinput.h>
#  pragma comment(lib, "xinput.lib")
#endif

namespace dae
{
	class Controller::Impl
	{
	public:
		explicit Impl(unsigned int index) : m_Index(index) {}

		void Update()
		{
#if defined(__EMSCRIPTEN__)

			m_PreviousButtons = m_CurrentButtons;
			m_CurrentButtons = 0;

			SDL_Gamepad* pad = SDL_GetGamepadFromID(static_cast<SDL_JoystickID>(m_Index));
			if (!pad) { m_Connected = false; return; }
			m_Connected = true;

			// Map SDL buttons → our ControllerButton bitmask
			auto sdlBtn = [&](SDL_GamepadButton btn, uint32_t bit)
				{
					if (SDL_GetGamepadButton(pad, btn))
						m_CurrentButtons |= bit;
				};
			sdlBtn(SDL_GAMEPAD_BUTTON_DPAD_UP, static_cast<uint32_t>(ControllerButton::DPadUp));
			sdlBtn(SDL_GAMEPAD_BUTTON_DPAD_DOWN, static_cast<uint32_t>(ControllerButton::DPadDown));
			sdlBtn(SDL_GAMEPAD_BUTTON_DPAD_LEFT, static_cast<uint32_t>(ControllerButton::DPadLeft));
			sdlBtn(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, static_cast<uint32_t>(ControllerButton::DPadRight));
			sdlBtn(SDL_GAMEPAD_BUTTON_START, static_cast<uint32_t>(ControllerButton::Start));
			sdlBtn(SDL_GAMEPAD_BUTTON_BACK, static_cast<uint32_t>(ControllerButton::Back));
			sdlBtn(SDL_GAMEPAD_BUTTON_LEFT_STICK, static_cast<uint32_t>(ControllerButton::LeftThumb));
			sdlBtn(SDL_GAMEPAD_BUTTON_RIGHT_STICK, static_cast<uint32_t>(ControllerButton::RightThumb));
			sdlBtn(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, static_cast<uint32_t>(ControllerButton::LeftShoulder));
			sdlBtn(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, static_cast<uint32_t>(ControllerButton::RightShoulder));
			sdlBtn(SDL_GAMEPAD_BUTTON_SOUTH, static_cast<uint32_t>(ControllerButton::ButtonA));
			sdlBtn(SDL_GAMEPAD_BUTTON_EAST, static_cast<uint32_t>(ControllerButton::ButtonB));
			sdlBtn(SDL_GAMEPAD_BUTTON_WEST, static_cast<uint32_t>(ControllerButton::ButtonX));
			sdlBtn(SDL_GAMEPAD_BUTTON_NORTH, static_cast<uint32_t>(ControllerButton::ButtonY));
#else

			m_PreviousButtons = m_CurrentButtons;
			m_CurrentButtons = 0;

			XINPUT_STATE state{};
			if (XInputGetState(m_Index, &state) != ERROR_SUCCESS)
			{
				m_Connected = false;
				return;
			}
			m_Connected = true;
			m_CurrentButtons = state.Gamepad.wButtons;
#endif
		}

		bool IsDown(uint32_t button)    const { return  (m_CurrentButtons & button) && !(m_PreviousButtons & button); }
		bool IsUp(uint32_t button)      const { return !(m_CurrentButtons & button) && (m_PreviousButtons & button); }
		bool IsPressed(uint32_t button) const { return  (m_CurrentButtons & button) != 0; }
		bool IsConnected()              const { return m_Connected; }

	private:
		unsigned int m_Index{};
		uint32_t     m_CurrentButtons{};
		uint32_t     m_PreviousButtons{};
		bool         m_Connected{ false };
	};

	//  Controller forwarding
	Controller::Controller(unsigned int controllerIndex)
		: m_pImpl(std::make_unique<Impl>(controllerIndex))
	{
	}

	Controller::~Controller() = default;

	Controller::Controller(Controller&&) noexcept = default;
	Controller& Controller::operator=(Controller&&) noexcept = default;

	void Controller::Update() { m_pImpl->Update(); }
	bool Controller::IsConnected()                          const { return m_pImpl->IsConnected(); }
	bool Controller::IsDown(ControllerButton b)             const { return m_pImpl->IsDown(static_cast<uint32_t>(b)); }
	bool Controller::IsUp(ControllerButton b)               const { return m_pImpl->IsUp(static_cast<uint32_t>(b)); }
	bool Controller::IsPressed(ControllerButton b)          const { return m_pImpl->IsPressed(static_cast<uint32_t>(b)); }
}