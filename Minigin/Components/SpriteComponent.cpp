#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Texture2D.h"

namespace dae
{
    SpriteComponent::SpriteComponent(GameObject* pOwner)
        : GameComponent(pOwner)
    {}

    void SpriteComponent::SetSpriteSheet(const std::string& filename)
    {
        m_Texture = ResourceManager::GetInstance().LoadTexture(filename);
    }

    void SpriteComponent::SetSpriteSheet(std::shared_ptr<Texture2D> texture)
    {
        m_Texture = std::move(texture);
    }

    void SpriteComponent::SetSourceRect(float x, float y, float w, float h)
    {
        m_SrcRect = { x, y, w, h };
    }

    void SpriteComponent::SetDestSize(float w, float h)
    {
        m_DestW = w;
        m_DestH = h;
        m_HasDestSize = true;
    }

    void SpriteComponent::ClearDestSize()
    {
        m_HasDestSize = false;
    }

    void SpriteComponent::Render() const
    {
        if (!m_Texture) return;

        auto* transform = GetOwner()->GetGameComponent<TransformComponent>();
        if (!transform) return;

        const auto& pos = transform->GetWorldPosition();

        const float dstW = m_HasDestSize ? m_DestW : m_SrcRect.w;
        const float dstH = m_HasDestSize ? m_DestH : m_SrcRect.h;

        Renderer::GetInstance().RenderTexture(*m_Texture,
            pos.x, pos.y,
            m_SrcRect,
            dstW, dstH);
    }
}
