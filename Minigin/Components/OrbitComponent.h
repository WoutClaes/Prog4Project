#pragma once
#include "GameComponent.h"

namespace dae
{
	class OrbitComponent final : public GameComponent
	{
	public:
		explicit OrbitComponent(GameObject* owner, float radius = 50.f, float speed = 1.f)
			: GameComponent(owner)
			, m_radius(radius)
			, m_speed(speed)
			, m_angle(0.f)
		{
		}

		~OrbitComponent() override = default;

		void Update() override;
		void FixedUpdate() override {}
		void Render() const override {}

		OrbitComponent(const OrbitComponent&) = delete;
		OrbitComponent(OrbitComponent&&) = delete;
		OrbitComponent& operator=(const OrbitComponent&) = delete;
		OrbitComponent& operator=(OrbitComponent&&) = delete;

	private:
		float m_radius;
		float m_speed;
		float m_angle;
	};
}