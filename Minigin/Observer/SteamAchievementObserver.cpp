#include "SteamAchievementObserver.h"
#include "Observer/Event.h"

#ifdef USE_STEAMWORKS
#pragma warning(push)
#pragma warning(disable:4996)
#include <steam_api.h>
#pragma warning(pop)
#endif

namespace dae
{
    void SteamAchievementObserver::OnNotify(GameObject*, Event event)
    {
        if (event != Events::GameWon)
            return;

#ifdef USE_STEAMWORKS
        if (SteamUserStats())
        {
            SteamUserStats()->SetAchievement("ACH_WIN_ONE_GAME");
            SteamUserStats()->StoreStats();
        }
#endif
    }
}