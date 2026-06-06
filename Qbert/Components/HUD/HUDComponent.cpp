#include "HUDComponent.h"
#include "GameManager.h"
#include "Components/TextComponent.h"
#include "Components/SpriteComponent.h"
#include <string>
#include <sstream>
#include <iomanip>

namespace qbert
{
    HUDComponent::HUDComponent(dae::GameObject* pOwner,
        dae::TextComponent* pScoreText,
        dae::TextComponent* m_pRoundText,
        dae::TextComponent* pLevelText,
        dae::SpriteComponent* pGameOverSprite,
        dae::SpriteComponent* pWinSprite,
        dae::TextComponent* pBonusText)
        : GameComponent(pOwner)
        , m_pScoreText(pScoreText)
        , m_pRoundText(m_pRoundText)
        , m_pLevelText(pLevelText)
        , m_pGameOverSprite(pGameOverSprite)
        , m_pWinSprite(pWinSprite)
        , m_pBonusText(pBonusText)
    {}

    void HUDComponent::Update()
    {
        auto& gm = GameManager::GetInstance();

        if (gm.GetScore() != m_PreviousScore && m_pScoreText)
        {
            m_PreviousScore = gm.GetScore();
            m_pScoreText->SetText(std::to_string(m_PreviousScore));
        }

        if (gm.GetRound() != m_PreviousLives && m_pRoundText)
        {
            m_PreviousLives = gm.GetRound();
            m_pRoundText->SetText("ROUND: " + std::to_string(gm.GetRound() + 1));
        }

        if (gm.GetLevel() != m_PreviousLevel && m_pLevelText)
        {
            m_PreviousLevel = gm.GetLevel();
            m_pLevelText->SetText("LEVEL: " + std::to_string(m_PreviousLevel));
        }

        if (m_pBonusText)
        {
            if (gm.m_Bonus != m_PreviousBonus)
            {
                m_PreviousBonus = gm.m_Bonus;
                m_pBonusText->SetText("BONUS   " + std::to_string(m_PreviousBonus));
            }
            else if (m_PreviousBonus != -1)
            {
                m_pBonusText->SetText("");
                m_PreviousBonus = -1;
            }
        }

        if (gm.IsGameOver() && !m_GameOverTriggered)
        {
            m_GameOverTriggered = true;

            if (gm.GetLives() <= 0 && m_pGameOverSprite)
            {
                m_pGameOverSprite->SetSpriteSheet("Game Over Title.png");
            }
            else if (m_pWinSprite)
            {
                m_pWinSprite->SetSpriteSheet("Victory Title.png");
            }
        }
    }
}