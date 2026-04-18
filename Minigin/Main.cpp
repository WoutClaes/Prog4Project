#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

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
#include "ResourceManager.h"
#include "Scene.h"
#include "InputManager.h"
#include "GameObject.h"
#include <glm/glm.hpp>
#include <filesystem>

#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Components/TextComponent.h"
#include "Components/FPSComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ScoreComponent.h"
#include "Components/LivesDisplayComponent.h"
#include "Components/ScoreDisplayComponent.h"
#include "Components/SpriteComponent.h"

#include "Commands/MoveCommand.h"
#include "Commands/DamageCommand.h"
#include "Commands/AddPointsCommand.h"
#include "Commands/ResetAchievementsCommand.h"

#include "Observer/SteamAchievementObserver.h"

namespace fs = std::filesystem;

static dae::GameObject* MakeLivesDisplay(dae::Scene& scene,
    std::shared_ptr<dae::Font> font,
    float x, float y,
    dae::HealthComponent* health)
{
    auto go = std::make_unique<dae::GameObject>();
    go->AddGameComponent<dae::TransformComponent>()->SetPosition(x, y);
    go->AddGameComponent<dae::TextComponent>("# lives: " + std::to_string(health->GetLives()), font);
    auto* display = go->AddGameComponent<dae::LivesDisplayComponent>();
    health->AddObserver(display);
    auto* ptr = go.get();
    scene.Add(std::move(go));
    return ptr;
}

static dae::GameObject* MakeScoreDisplay(dae::Scene& scene,
    std::shared_ptr<dae::Font> font,
    float x, float y,
    dae::ScoreComponent* score)
{
    auto go = std::make_unique<dae::GameObject>();
    go->AddGameComponent<dae::TransformComponent>()->SetPosition(x, y);
    go->AddGameComponent<dae::TextComponent>("Score: 0", font);
    auto* display = go->AddGameComponent<dae::ScoreDisplayComponent>();
    score->AddObserver(display);
    auto* ptr = go.get();
    scene.Add(std::move(go));
    return ptr;
}

static void MakeSteamAchievement(dae::Scene& scene, dae::ScoreComponent* score)
{
    auto go = std::make_unique<dae::GameObject>();
    auto* observer = go->AddGameComponent<dae::SteamAchievementObserver>();
    score->AddObserver(observer);
    scene.Add(std::move(go));
}

static void load(bool steamInitialized)
{
    (void)steamInitialized;
    auto& scene = dae::SceneManager::GetInstance().CreateScene();

    (void)&MakeLivesDisplay;
    (void)&MakeScoreDisplay;
    (void)&MakeSteamAchievement;
    (void)scene;
}

int main(int, char* [])
{
#ifdef USE_STEAMWORKS
    const bool steamInitialized = SteamAPI_Init();
    if (!steamInitialized)
        SDL_Log("Steam not initialized - achievements will be disabled.");
#endif

#if __EMSCRIPTEN__
    fs::path data_location = "";
#else
    fs::path data_location = "./Data/";
    if (!fs::exists(data_location))
        data_location = "../Data/";
#endif

    dae::Minigin engine(data_location);
#ifdef USE_STEAMWORKS
    engine.Run([steamInitialized]() { load(steamInitialized); });
#else
    engine.Run([]() { load(false); });
#endif

#ifdef USE_STEAMWORKS
    if (steamInitialized)
        SteamAPI_Shutdown();
#endif

    return 0;
}
