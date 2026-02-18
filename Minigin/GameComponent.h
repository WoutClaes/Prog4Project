#pragma once

namespace dae
{
	class GameObject;

	class GameComponent
	{
	public:
		explicit GameComponent(GameObject* pOwner)
			:m_pOwner{ pOwner }
		{};
		virtual ~GameComponent() = default;

		virtual void Update() {};
		virtual void FixedUpdate() {};
		virtual void Render() const {};

		GameComponent(const GameComponent&) = delete;
		GameComponent& operator=(const GameComponent&) = delete;
		GameComponent(GameComponent&&) = delete;
		GameComponent& operator=(GameComponent&&) = delete;
	protected:
		GameObject* GetOwner() const { return m_pOwner; };

	private:
		GameObject* m_pOwner{ nullptr };
	};
}