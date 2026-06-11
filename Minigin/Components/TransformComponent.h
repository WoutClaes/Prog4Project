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
		{
		}
		~TransformComponent() override = default;

		void SetLocalPosition(float x, float y, float z = 0.f)
		{
			m_localTransform.SetPosition(x, y, z);
			SetDirty();
		}

		void SetLocalPosition(const glm::vec3& pos)
		{
			m_localTransform.SetPosition(pos);
			SetDirty();
		}

		void SetPosition(float x, float y, float z = 0.f) { SetLocalPosition(x, y, z); }

		const Transform& GetLocalTransform() const { return m_localTransform; }

		const Transform& GetWorldTransform();

		const glm::vec3& GetWorldPosition() { return GetWorldTransform().GetPosition(); }

		const Transform& GetTransform() { return GetWorldTransform(); }

		void SetDirty() { m_isDirty = true; }

		void Update() override {}
		void FixedUpdate() override {}
		void Render() const override {}

		TransformComponent(const TransformComponent&) = delete;
		TransformComponent(TransformComponent&&) = delete;
		TransformComponent& operator=(const TransformComponent&) = delete;
		TransformComponent& operator=(TransformComponent&&) = delete;

	private:
		void RecalculateWorldTransform();

		Transform m_localTransform{};
		Transform m_worldTransform{};
		bool m_isDirty{ true };
	};
}