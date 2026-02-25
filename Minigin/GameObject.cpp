#include <string>
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Components/TransformComponent.h"

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

void dae::GameObject::SetParent(GameObject* parent, bool keepWorldPosition)
{
	if (parent == m_parent)
		return;

	if (parent != nullptr)
	{
		GameObject* check = parent;
		while (check)
		{
			if (check == this) return;
			check = check->m_parent;
		}
	}

	if (m_parent)
		m_parent->RemoveChild(this);

	if (auto* transform = GetGameComponent<TransformComponent>())
	{
		if (keepWorldPosition && parent)
		{
			if (auto* parentTransform = parent->GetGameComponent<TransformComponent>())
			{
				glm::vec3 worldPos = transform->GetWorldPosition();
				glm::vec3 parentPos = parentTransform->GetWorldPosition();
				transform->SetLocalPosition(worldPos - parentPos);
			}
		}
		else if (!keepWorldPosition)
		{
			transform->SetLocalPosition({ 0.f, 0.f, 0.f });
		}
	}

	m_parent = parent;

	if (m_parent)
		m_parent->AddChild(this);

	SetPositionDirty();
}

dae::GameObject* dae::GameObject::GetChildAt(size_t index) const
{
	if (index >= m_children.size())
		return nullptr;
	return m_children[index];
}

void dae::GameObject::SetPositionDirty()
{
	if (auto* transform = GetGameComponent<TransformComponent>())
		transform->SetDirty();

	for (GameObject* child : m_children)
		child->SetPositionDirty();
}

void dae::GameObject::AddChild(GameObject* child)
{
	if (child && std::find(m_children.begin(), m_children.end(), child) == m_children.end())
		m_children.push_back(child);
}

void dae::GameObject::RemoveChild(GameObject* child)
{
	m_children.erase(std::remove(m_children.begin(), m_children.end(), child), m_children.end());
}
