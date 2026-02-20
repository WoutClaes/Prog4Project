#include <algorithm>
#include "Scene.h"
#include <cassert>

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	m_GameObjects.emplace_back(std::move(object));
}

void Scene::Remove(const GameObject& object)
{
	m_GameObjectsToRemove.push_back(&object);
}

void Scene::CleanupRemovedGameObjects()
{
	for (const GameObject* toRemove : m_GameObjectsToRemove)
	{
		m_GameObjects.erase(
			std::remove_if(m_GameObjects.begin(), m_GameObjects.end(),
				[toRemove](const std::unique_ptr<GameObject>& obj)
				{
					return obj.get() == static_cast<const GameObject*>(toRemove);
				}),
			m_GameObjects.end()
		);
	}
	m_GameObjectsToRemove.clear();
}

void Scene::RemoveAll()
{
	m_GameObjects.clear();
}

void Scene::Update()
{
	for(auto& object : m_GameObjects)
	{
		object->Update();
	}

	CleanupRemovedGameObjects();
}

void dae::Scene::FixedUpdate()
{
	for (auto& object : m_GameObjects)
		object->FixedUpdate();
}

void Scene::Render() const
{
	for (const auto& object : m_GameObjects)
	{
		object->Render();
	}
}
