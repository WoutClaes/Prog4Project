#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "Scene.h"
#include "Singleton.h"

namespace dae
{
	class Scene;
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateScene();

		void Update();
		void FixedUpdate();
		void Render();

		void RemoveAllScenes();
		Scene& GetActiveScene();

		void QueueAction(std::function<void()> action);

	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;

		void ExecutePendingActions();

		std::vector<std::unique_ptr<Scene>> m_scenes{};
		std::vector<std::function<void()>> m_pendingActions{};
	};
}
