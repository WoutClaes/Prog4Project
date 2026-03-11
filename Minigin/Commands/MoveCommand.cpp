#include "MoveCommand.h"
#include "GameObject.h"
#include "Components/TransformComponent.h"
#include "GameTime.h"

namespace dae
{
	MoveCommand::MoveCommand(GameObject* pGameObject, glm::vec3 direction, float speed)
		: GameObjectCommand(pGameObject)
		, m_Direction(direction)
		, m_Speed(speed)
	{

	}

	void MoveCommand::Execute()
	{
		auto* pTransform = GetGameObject()->GetGameComponent<TransformComponent>();
		if (!pTransform)
			return;

		const float dt = GameTime::GetInstance().GetDeltaTime();
		const glm::vec3 delta = m_Direction * m_Speed * dt;

		const glm::vec3 currentPos = pTransform->GetLocalTransform().GetPosition();
		pTransform->SetLocalPosition(currentPos + delta);
	}
}