#pragma once
#include "GameComponent.h"

namespace dae
{
	class TextComponent;
	class FPSComponent final : public GameComponent
	{
	public:
		explicit FPSComponent(GameObject* pOwner) 
			: GameComponent(pOwner) 
		{};

		void Update() override;

	private:
		float m_AccumulatedTime{ 0.f };
		int m_FrameCount{ 0 };
		float m_FPS{ 0 };
	};
}