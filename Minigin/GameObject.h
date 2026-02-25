#pragma once
#include <vector>
#include <memory>
#include <type_traits>
#include "Components/GameComponent.h"
#include <algorithm>

namespace dae
{
	class GameObject final
	{
	public:
		void Update();
		void FixedUpdate();
		void Render() const;

		template<typename T, typename... Args>
		T* AddGameComponent(Args&&... args)
		{
			static_assert(std::is_base_of_v<GameComponent, T>,
				"T must derive from GameComponent");

			auto gameComponent = std::make_unique<T>(this, std::forward<Args>(args)...);
			T* gameComponentPtr = gameComponent.get();
			m_GameComponents.push_back(std::move(gameComponent));
			return gameComponentPtr;
		}

		template<typename T>
		void RemoveGameComponent()
		{
			static_assert(std::is_base_of_v<GameComponent, T>,
				"T must derive from GameComponent");

			auto it = std::find_if(m_GameComponents.begin(), m_GameComponents.end(),
				[](const std::unique_ptr<GameComponent>& c)
				{
					return dynamic_cast<T*>(c.get()) != nullptr;
				});

			if (it != m_GameComponents.end())
				m_GameComponentsToRemove.push_back(it->get());
		}

		template<typename T>
		T* GetGameComponent() const
		{
			static_assert(std::is_base_of_v<GameComponent, T>,
				"T must derive from GameComponent");

			for (auto& c : m_GameComponents)
			{
				if (auto* casted = dynamic_cast<T*>(c.get()))
					return casted;
			}
			return nullptr;
		}

		template<typename T>
		bool HasGameComponent() const
		{
			return GetGameComponent<T>() != nullptr;
		}

		void SetParent(GameObject* parent, bool keepWorldPosition = true);

		GameObject* GetParent() const { return m_parent; }

		size_t GetChildCount() const { return m_children.size(); }
		GameObject* GetChildAt(size_t index) const;

		const std::vector<GameObject*>& GetChildren() const { return m_children; }

		void SetPositionDirty();

		GameObject() = default;
		~GameObject();

		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

	private:
		void CleanupRemovedGameComponents();

		void AddChild(GameObject* child);
		void RemoveChild(GameObject* child);

		std::vector<std::unique_ptr<GameComponent>> m_GameComponents;
		std::vector<GameComponent*> m_GameComponentsToRemove;

		GameObject* m_parent{ nullptr };
		std::vector<GameObject*> m_children{};
	};
}
