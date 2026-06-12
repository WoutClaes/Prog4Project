#include "QbertRenderComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "Components/TransformComponent.h"
#include "GameObject.h"

namespace qbert
{
    QbertRenderComponent::QbertRenderComponent(dae::GameObject* pOwner, QbertComponent* pQbert, int playerIdx)
        : GameComponent(pOwner)
        , m_PlayerIdx(playerIdx)
    {
        if (m_PlayerIdx == 0)
        {
            m_Texture = dae::ResourceManager::GetInstance().LoadTexture("Qbert P1 Spritesheet.png");
        }
        else
        {
            m_Texture = dae::ResourceManager::GetInstance().LoadTexture("Qbert P2 Spritesheet.png");
        }

        if (pQbert)
        {
            pQbert->OnJumpStarted = [this](JumpDirection dir)
                {
                    m_LastDir = dir;
                };
        }
    }

    void QbertRenderComponent::Render() const
    {
        if (!m_Texture) return;

        auto* transform = GetOwner()->GetGameComponent<dae::TransformComponent>();
        if (!transform) return;

        // DownLeft, DownRight, UpLeft, UpRight
        static constexpr int FrameForDir[4] = { 3, 2, 1, 0 };
        const int frame = FrameForDir[static_cast<int>(m_LastDir)];
        const float srcX  = frame * SrcW;
        const float srcY  = 0.f;

        const SDL_FRect src{ srcX, srcY, SrcW, SrcH };

        const auto& pos = transform->GetWorldPosition();
        dae::Renderer::GetInstance().RenderTexture(*m_Texture, pos.x, pos.y, src, SrcW * Scale, SrcH * Scale);
    }
}
