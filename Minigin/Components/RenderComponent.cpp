#include "RenderComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"

namespace dae
{
	void RenderComponent::SetTexture(const std::string& filename)
	{
		m_texture = ResourceManager::GetInstance().LoadTexture(filename);
	}

	void RenderComponent::SetTexture(std::shared_ptr<Texture2D> texture)
	{
		m_texture = std::move(texture);
	}

	void RenderComponent::Render() const
	{
		if (!m_texture) return;

		if (auto* transform = GetOwner()->GetGameComponent<TransformComponent>())
		{
			const auto& pos = transform->GetWorldPosition();
			Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
		}
	}
}