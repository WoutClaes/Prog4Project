#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <filesystem>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#ifdef USE_STEAMWORKS
#pragma warning(push)
#pragma warning(disable:4996)
#include <steam_api.h>
#pragma warning(pop)
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "Scene.h"

namespace fs = std::filesystem;

static void load(bool /*steamInitialized*/)
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene();
    (void)scene;
    // Add game content here
}

int main(int, char* [])
{
#ifdef USE_STEAMWORKS
    const bool steamInitialized = SteamAPI_Init();
    if (!steamInitialized)
        SDL_Log("Steam not initialized.");
#else
    const bool steamInitialized = false;
#endif

#if __EMSCRIPTEN__
    fs::path dataLocation = "";
#else
    fs::path dataLocation = "./Data/";
    if (!fs::exists(dataLocation))
        dataLocation = "../Data/";
#endif

    dae::Minigin engine(dataLocation);
    engine.Run([steamInitialized]() { load(steamInitialized); });

#ifdef USE_STEAMWORKS
    if (steamInitialized)
        SteamAPI_Shutdown();
#endif

    return 0;
}
