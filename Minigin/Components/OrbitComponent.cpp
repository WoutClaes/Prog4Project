#include "OrbitComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "GameTime.h"
#include <cmath>

namespace dae
{
	void OrbitComponent::Update()
	{
		const float dt = GameTime::GetInstance().GetDeltaTime();
		m_angle += m_speed * dt;

		constexpr float twoPi = 6.28318530718f;
		if (m_angle > twoPi) m_angle -= twoPi;
		if (m_angle < -twoPi) m_angle += twoPi;

		if (auto* transform = GetOwner()->GetGameComponent<TransformComponent>())
		{
			transform->SetLocalPosition(
				std::cos(m_angle) * m_radius,
				std::sin(m_angle) * m_radius,
				0.f
			);
		}
	}
}