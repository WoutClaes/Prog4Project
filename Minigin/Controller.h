#pragma once
#include <memory>
#include <cstdint>

namespace dae
{
	enum class ControllerButton : uint32_t
	{
		DPadUp = 0x0001,
		DPadDown = 0x0002,
		DPadLeft = 0x0004,
		DPadRight = 0x0008,
		Start = 0x0010,
		Back = 0x0020,
		LeftThumb = 0x0040,
		RightThumb = 0x0080,
		LeftShoulder = 0x0100,
		RightShoulder = 0x0200,
		ButtonA = 0x1000,
		ButtonB = 0x2000,
		ButtonX = 0x4000,
		ButtonY = 0x8000,
	};

	enum class KeyState
	{
		Down,
		Up,
		Pressed,
	};

	class Controller final
	{
	public:
		explicit Controller(unsigned int controllerIndex);
		~Controller();

		void Update();

		bool IsDown(ControllerButton button) const;
		bool IsUp(ControllerButton button) const;
		bool IsPressed(ControllerButton button) const;

		bool IsConnected() const;

		Controller(const Controller&) = delete;
		Controller& operator=(const Controller&) = delete;
		Controller(Controller&&) noexcept;
		Controller& operator=(Controller&&) noexcept;

	private:
		class Impl;
		std::unique_ptr<Impl> m_pImpl;
	};
}