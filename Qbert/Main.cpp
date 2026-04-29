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
#include "InputManager.h"
#include "Components/TransformComponent.h"
#include "Sound/ServiceLocator.h"
#include "Sound/SDLSoundSystem.h"

#include "Grid/CubeGrid.h"

#include "Components/Grid/CubeGridComponent.h"
#include "Components/Grid/GridRenderComponent.h"
#include "Components/Player/QbertComponent.h"
#include "Components/Player/QbertRenderComponent.h"
#include "Commands/JumpCommand.h"
#include "Components/ScoreComponent.h"

#include "Observer/SteamAchievementObserver.h"

namespace fs = std::filesystem;

static void load(bool steamInitialized)
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene();

    dae::ServiceLocator::RegisterSoundSystem(
        std::make_unique<dae::SDLSoundSystem>()
    );

    // --- Grid ---
    constexpr float originX = 400.f;
    constexpr float originY =  50.f;
    constexpr float scale   =   2.f;

    auto gridObject = std::make_unique<dae::GameObject>();
    gridObject->AddGameComponent<dae::TransformComponent>()->SetPosition(0.f, 0.f);
    auto* gridComp   = gridObject->AddGameComponent<qbert::CubeGridComponent>(originX, originY, scale);
    auto* renderComp = gridObject->AddGameComponent<qbert::GridRenderComponent>(gridComp->GetGrid());
    renderComp->SetBaseColorIndex(1);
    renderComp->SetIntermediateColorIndex(2);
    renderComp->SetTargetColorIndex(3);

    // --- Qbert ---
    auto qbertObject = std::make_unique<dae::GameObject>();
    qbertObject->AddGameComponent<dae::TransformComponent>()->SetPosition(0.f, 0.f);
    auto* qbertComp  = qbertObject->AddGameComponent<qbert::QbertComponent>(gridComp->GetGrid(), 0, 0);
    auto* qbertRender = qbertObject->AddGameComponent<qbert::QbertRenderComponent>(qbertComp);

    (void)qbertRender;

    // --- Score + Steam Achievement ---
    auto* scoreComp = qbertObject->AddGameComponent<dae::ScoreComponent>();

#ifdef USE_STEAMWORKS
    if (steamInitialized)
    {
        auto* achievement = qbertObject->AddGameComponent<dae::SteamAchievementObserver>();
        scoreComp->AddObserver(achievement);
    }
#endif

    // --- Input: keyboard ---
    auto& input = dae::InputManager::GetInstance();

    input.BindKeyboardCommand(SDLK_KP_7, dae::KeyState::Down,
        std::make_unique<qbert::JumpCommand>(qbertObject.get(), qbert::JumpDirection::UpLeft));

    input.BindKeyboardCommand(SDLK_KP_9, dae::KeyState::Down,
        std::make_unique<qbert::JumpCommand>(qbertObject.get(), qbert::JumpDirection::UpRight));

    input.BindKeyboardCommand(SDLK_KP_1, dae::KeyState::Down,
        std::make_unique<qbert::JumpCommand>(qbertObject.get(), qbert::JumpDirection::DownLeft));

    input.BindKeyboardCommand(SDLK_KP_3, dae::KeyState::Down,
        std::make_unique<qbert::JumpCommand>(qbertObject.get(), qbert::JumpDirection::DownRight));


    // --- Input: gamepad (controller 0) ---
    input.BindControllerCommand(0, dae::ControllerButton::DPadLeft,  dae::KeyState::Down,
        std::make_unique<qbert::JumpCommand>(qbertObject.get(), qbert::JumpDirection::UpLeft));

    input.BindControllerCommand(0, dae::ControllerButton::DPadRight, dae::KeyState::Down,
        std::make_unique<qbert::JumpCommand>(qbertObject.get(), qbert::JumpDirection::DownRight));

    input.BindControllerCommand(0, dae::ControllerButton::DPadUp,    dae::KeyState::Down,
        std::make_unique<qbert::JumpCommand>(qbertObject.get(), qbert::JumpDirection::UpRight));

    input.BindControllerCommand(0, dae::ControllerButton::DPadDown,  dae::KeyState::Down,
        std::make_unique<qbert::JumpCommand>(qbertObject.get(), qbert::JumpDirection::DownLeft));

    scene.Add(std::move(gridObject));
    scene.Add(std::move(qbertObject));
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
