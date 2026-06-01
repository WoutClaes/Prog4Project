#include "SceneManager.h"
#include "SceneManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include <cassert>

void dae::SceneManager::Update()
{
	for(auto& scene : m_scenes)
	{
		scene->Update();
	}

	ExecutePendingActions();
}

void dae::SceneManager::FixedUpdate()
{
	for (const auto& scene : m_scenes)
	{
		scene->FixedUpdate();
	}
}

void dae::SceneManager::Render()
{
	for (const auto& scene : m_scenes)
	{
		scene->Render();
	}
}

void dae::SceneManager::RemoveAllScenes()
{
	m_scenes.clear();
}

dae::Scene& dae::SceneManager::GetActiveScene()
{
	assert(!m_scenes.empty() && "No scenes available");
	return *m_scenes.back();
}

dae::Scene& dae::SceneManager::CreateScene()
{
	m_scenes.emplace_back(std::unique_ptr<Scene>(new Scene()));
	return *m_scenes.back();
}

void dae::SceneManager::QueueAction(std::function<void()> action)
{
	m_pendingActions.push_back(std::move(action));
}

void dae::SceneManager::ExecutePendingActions()
{
	if (m_pendingActions.empty()) return;

	auto actions = std::move(m_pendingActions);
	m_pendingActions.clear();

	for (const auto& action : actions)
	{
		action();
	}
}
