#pragma once
#include "Command.h"
#include "GameObject.h"

namespace dae
{
	class GameObjectCommand : public Command
	{
	public:
		explicit GameObjectCommand (GameObject* pGameObject)
			: m_pGameObject(pGameObject) {}

		virtual ~GameObjectCommand() = default;

	protected:
		GameObject* GetGameObject() const { return m_pGameObject; };

	private:
		GameObject* m_pGameObject;
	};
}
