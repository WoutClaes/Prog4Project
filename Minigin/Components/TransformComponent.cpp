#include "TransformComponent.h"
#include "GameObject.h"

namespace dae
{
	const Transform& TransformComponent::GetWorldTransform()
	{
		if (m_isDirty)
			RecalculateWorldTransform();
		return m_worldTransform;
	}

	void TransformComponent::RecalculateWorldTransform()
	{
		GameObject* parent = GetOwner()->GetParent();
		if (parent)
		{
			if (auto* parentTransform = parent->GetGameComponent<TransformComponent>())
			{
				const glm::vec3& parentPos = parentTransform->GetWorldTransform().GetPosition();
				const glm::vec3& localPos = m_localTransform.GetPosition();
				m_worldTransform.SetPosition(parentPos + localPos);
			}
			else
			{
				m_worldTransform.SetPosition(m_localTransform.GetPosition());
			}
		}
		else
		{
			m_worldTransform.SetPosition(m_localTransform.GetPosition());
		}

		m_isDirty = false;
	}
}