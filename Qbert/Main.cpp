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
#include "GameObject.h"
#include "Components/TransformComponent.h"

#include "Grid/CubeGrid.h"
#include "Components/Grid/CubeGridComponent.h"
#include "Components/Grid/GridRenderComponent.h"

namespace fs = std::filesystem;

static void load(bool /*steamInitialized*/)
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene();

    // Window is 1024x576.
    // Pyramid at 32x32 tiles: ~224px wide, ~144px tall.
    // Center horizontally, leave top margin for HUD.
    constexpr float originX = 480.f;
    constexpr float originY =  80.f;

    auto gridObject = std::make_unique<dae::GameObject>();
    gridObject->AddGameComponent<dae::TransformComponent>()->SetPosition(0.f, 0.f);

    // CubeGrid is stored as a component so the scene owns its lifetime
    auto* gridComp = gridObject->AddGameComponent<qbert::CubeGridComponent>(originX, originY);

    auto* renderComp = gridObject->AddGameComponent<qbert::GridRenderComponent>(gridComp->GetGrid());
    renderComp->SetBaseColorIndex(1);         // orange
    renderComp->SetIntermediateColorIndex(2); // dark
    renderComp->SetTargetColorIndex(3);       // blue/purple

    scene.Add(std::move(gridObject));
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
