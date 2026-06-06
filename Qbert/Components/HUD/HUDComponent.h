#pragma once
#include "Components/GameComponent.h"
#include <memory>

namespace dae
{
    class TextComponent;
    class SpriteComponent;
}

namespace qbert
{
    class HUDComponent final : public dae::GameComponent
    {
    public:
        HUDComponent(dae::GameObject* pOwner,
            dae::TextComponent* pScoreText,
            dae::TextComponent* m_pRoundText,
            dae::TextComponent* pLevelText,
            dae::SpriteComponent* pGameOverSprite,
            dae::SpriteComponent* pWinSprite,
            dae::TextComponent* pBonusText);
        ~HUDComponent() override = default;

        void Update() override;
        void FixedUpdate() override {}
        void Render() const override {}

        HUDComponent(const HUDComponent&) = delete;
        HUDComponent& operator=(const HUDComponent&) = delete;
        HUDComponent(HUDComponent&&) = delete;
        HUDComponent& operator=(HUDComponent&&) = delete;

    private:
        dae::TextComponent* m_pScoreText{ nullptr };
        dae::TextComponent* m_pRoundText{ nullptr };
        dae::TextComponent* m_pLevelText{ nullptr };
        dae::SpriteComponent* m_pGameOverSprite{ nullptr };
        dae::SpriteComponent* m_pWinSprite{ nullptr };
        dae::TextComponent* m_pBonusText{ nullptr };

        int m_PreviousScore{ -1 };
        int m_PreviousLives{ -1 };
        int m_PreviousLevel{ -1 };
        int m_PreviousBonus{ -1 };
        int m_LastScore{ -1 };
        bool m_GameOverTriggered{ false };
    };
}
