#include "ScoreComponent.h"
#include "GameObject.h"

namespace dae
{
    ScoreComponent::ScoreComponent(GameObject* pOwner)
        : GameComponent(pOwner)
    {}

    void ScoreComponent::AddPoints(int amount)
    {
        m_Score += amount;
        Notify(GetOwner(), Events::PointsChanged);

        if (!m_WinAchievementGranted && m_Score >= WinScore)
        {
            m_WinAchievementGranted = true;
            Notify(GetOwner(), Events::GameWon);
        }
    }
}
