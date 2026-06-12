#include "SlickSamRenderComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "Components/TransformComponent.h"
#include "GameObject.h"

namespace qbert
{
    SlickSamRenderComponent::SlickSamRenderComponent(dae::GameObject* pOwner, SlickSamComponent* pSlickSam)
        : GameComponent(pOwner)
        , m_pSlickSam(pSlickSam)
    {
        m_Texture = dae::ResourceManager::GetInstance()
            .LoadTexture("Slick Sam Spritesheet.png");
    }

    void SlickSamRenderComponent::Render() const
    {
        if (!m_Texture || !m_pSlickSam) return;
        if (m_pSlickSam->IsDead()) return;

        auto* transform = GetOwner()->GetGameComponent<dae::TransformComponent>();
        if (!transform) return;

        const int frameIdx = static_cast<int>(m_pSlickSam->GetCurrentFrame());
        const float srcX = (frameIdx % 2) * TileW;
        const float srcY = (frameIdx / 2) * TileH;

        const SDL_FRect src{ srcX, srcY, TileW, TileH };
        const auto& pos = transform->GetWorldPosition();

        dae::Renderer::GetInstance().RenderTexture(*m_Texture, pos.x, pos.y, src, TileW * Scale, TileH * Scale);
    }
}
