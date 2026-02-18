#include <string>
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"

dae::GameObject::~GameObject() = default;

void dae::GameObject::Update()
{
	for (auto& component : m_GameComponents)
		component->Update();

	CleanupRemovedGameComponents();
}

void dae::GameObject::FixedUpdate()
{
	for (auto& component : m_GameComponents)
		component->FixedUpdate();
}

void dae::GameObject::Render() const
{
	for (auto& component : m_GameComponents)
		component->Render();
}

void dae::GameObject::CleanupRemovedGameComponents()
{
	for (GameComponent* toRemove : m_GameComponentsToRemove)
	{
		m_GameComponents.erase(
			std::remove_if(m_GameComponents.begin(), m_GameComponents.end(),
				[toRemove](const std::unique_ptr<GameComponent>& c)
				{
					return c.get() == toRemove;
				}),
			m_GameComponents.end()
		);
	}
	m_GameComponentsToRemove.clear();
}