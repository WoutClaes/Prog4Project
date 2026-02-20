#pragma once
#include "GameComponent.h"
#include "Transform.h"

namespace dae
{
	class TransformComponent : public GameComponent
	{
	public:
		explicit TransformComponent(GameObject* pOwner)
			: GameComponent(pOwner)
		{};

		void SetPosition(float x, float y) { m_Transform.SetPosition(x, y, 0.f); }
		const Transform& GetTransform() const { return m_Transform; };

	private:
		Transform m_Transform{};
	};
}