#include "CoilyRenderComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "Components/TransformComponent.h"
#include "GameObject.h"

namespace qbert
{
    CoilyRenderComponent::CoilyRenderComponent(dae::GameObject* pOwner,
                                               CoilyComponent* pCoily)
        : GameComponent(pOwner)
        , m_pCoily(pCoily)
    {
        m_Texture = dae::ResourceManager::GetInstance()
            .LoadTexture("Coily Spritesheet.png");
    }

    void CoilyRenderComponent::Render() const
    {
        if (!m_Texture || !m_pCoily || m_pCoily->IsDead()) return;

        auto* transform = GetOwner()->GetGameComponent<dae::TransformComponent>();
        if (!transform) return;

        const int   frame = static_cast<int>(m_pCoily->GetCurrentFrame());
        const float srcX  = frame * SrcW;
        const float srcY  = 0.f;

        const SDL_FRect src{ srcX, srcY, SrcW, SrcH };

        const auto& pos = transform->GetWorldPosition();
        dae::Renderer::GetInstance().RenderTexture(
            *m_Texture,
            pos.x, pos.y,
            src,
            SrcW * Scale,
            SrcH * Scale
        );
    }
}
