#include "ResetAchievementsCommand.h"

#ifdef USE_STEAMWORKS
#pragma warning(push)
#pragma warning(disable:4996)
#include <steam_api.h>
#pragma warning(pop)
#endif

#include <SDL3/SDL.h>

namespace dae
{
    void ResetAchievementsCommand::Execute()
    {
#ifdef USE_STEAMWORKS
        if (SteamUserStats())
        {
            SteamUserStats()->ResetAllStats(true);
            SteamUserStats()->StoreStats();
            SDL_Log("Steam achievements reset.");
        }
#endif
    }
}
