#include "DiskRenderComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "Components/TransformComponent.h"
#include "GameObject.h"
#include "GameTime.h"

namespace qbert
{
    DiskRenderComponent::DiskRenderComponent(dae::GameObject* pOwner)
        : GameComponent(pOwner)
    {
        m_Texture = dae::ResourceManager::GetInstance().LoadTexture("Disks.png");
    }

    void DiskRenderComponent::Update()
    {
        if (!GetOwner()->IsActive()) return;

        m_AnimTimer += dae::GameTime::GetInstance().GetDeltaTime();
        if (m_AnimTimer >= AnimSpeed)
        {
            m_AnimTimer -= AnimSpeed;
            m_CurrentFrame = (m_CurrentFrame + 1) % MaxFrames;
        }
    }

    void DiskRenderComponent::Render() const
    {
        if (!GetOwner()->IsActive() || !m_Texture) return;

        auto* transform = GetOwner()->GetGameComponent<dae::TransformComponent>();
        if (!transform) return;

        const float srcX = m_CurrentFrame * SrcW;
        const float srcY = 0.f;
        const SDL_FRect src{ srcX, srcY, SrcW, SrcH };

        const auto& pos = transform->GetWorldPosition();
        dae::Renderer::GetInstance().RenderTexture(*m_Texture, pos.x, pos.y, src, SrcW * Scale, SrcH * Scale);
    }
}