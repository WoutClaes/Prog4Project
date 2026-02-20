#include "FPSComponent.h"
#include "TextComponent.h"
#include "GameTime.h"
#include "GameObject.h"
#include <format>

void dae::FPSComponent::Update()
{
	m_AccumulatedTime += GameTime::GetInstance().GetDeltaTime();
	++m_FrameCount;

	if (m_AccumulatedTime >= 0.1f)
	{
		m_FPS = static_cast<float>(m_FrameCount) / m_AccumulatedTime;
		m_AccumulatedTime = 0.f;
		m_FrameCount = 0;

		auto* text = GetOwner()->GetGameComponent<TextComponent>();
		if (text)
			text->SetText(std::format("{:.1f} FPS", m_FPS));
	}
}