#include "ScoreComponent.h"
#include "GameObject.h"

#ifdef USE_STEAMWORKS
#pragma warning(push)
#pragma warning(disable:4996)
#include <steam_api.h>
#pragma warning(pop)
#endif

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

#ifdef USE_STEAMWORKS
            if (SteamUserStats())
            {
                SteamUserStats()->SetAchievement("ACH_WIN_ONE_GAME");
                SteamUserStats()->StoreStats();
            }
#endif
        }
    }
}
