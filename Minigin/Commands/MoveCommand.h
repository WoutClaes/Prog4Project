#pragma once
#include "GameObjectCommand.h"
#include <glm/glm.hpp>

namespace dae
{
	class MoveCommand final : public GameObjectCommand
	{
	public:
		MoveCommand(GameObject* pGameObject, glm::vec3 direction, float speed);

		void Execute() override;

	private:
		glm::vec3 m_Direction;
		float m_Speed;
	};
}
