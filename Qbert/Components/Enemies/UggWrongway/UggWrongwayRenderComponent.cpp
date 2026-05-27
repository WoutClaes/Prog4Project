#include "UggWrongwayRenderComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "Components/TransformComponent.h"
#include "GameObject.h"

namespace qbert
{
    UggWrongwayRenderComponent::UggWrongwayRenderComponent(dae::GameObject* pOwner, UggWrongwayComponent* pUggWrongway)
        : GameComponent(pOwner)
        , m_pUggWrongway(pUggWrongway)
    {
        m_Texture = dae::ResourceManager::GetInstance().LoadTexture("Ugg Wrongway Spritesheet.png");
    }

    void UggWrongwayRenderComponent::Render() const
    {
        if (!m_Texture || !m_pUggWrongway) return;
        if (m_pUggWrongway->IsDead()) return;

        auto* transform = GetOwner()->GetGameComponent<dae::TransformComponent>();
        if (!transform) return;

        const int frameIdx = static_cast<int>(m_pUggWrongway->GetCurrentFrame());

        const float srcX = (frameIdx % 4) * TileW;
        const float srcY = (frameIdx / 4) * TileH;

        const SDL_FRect src{ srcX, srcY, TileW, TileH };
        const auto& pos = transform->GetWorldPosition();

        dae::Renderer::GetInstance().RenderTexture(
            *m_Texture,
            pos.x, pos.y,
            src,
            TileW * Scale,
            TileH * Scale
        );
    }
}
