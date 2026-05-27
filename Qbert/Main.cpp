// Standard Library
#include <filesystem>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

// Third‑Party Libraries
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#ifdef USE_STEAMWORKS
#pragma warning(push)
#pragma warning(disable:4996)
#include <steam_api.h>
#pragma warning(pop)
#endif

// Engine Core
#include "Minigin.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "InputManager.h"

// Core Components
#include "Components/TransformComponent.h"
#include "Components/ScoreComponent.h"

// Grid System
#include "Grid/CubeGrid.h"
#include "Components/Grid/CubeGridComponent.h"
#include "Components/Grid/GridRenderComponent.h"
#include "Components/Grid/GridMover.h"

// Player / Qbert
#include "Components/Player/QbertComponent.h"
#include "Components/Player/QbertRenderComponent.h"
#include "Commands/JumpCommand.h"

// Enemy / Coily
#include "Components/Enemies/Coily/CoilyComponent.h"
#include "Components/Enemies/Coily/CoilyRenderComponent.h"

// Enemy / Slick and Sam
#include "Components/Enemies/SlickSam/SlickSamComponent.h"
#include "Components/Enemies/SlickSam/SlickSamRenderComponent.h"

// Enemy / Ugg and Wrongway
#include "Components/Enemies/UggWrongway/UggWrongwayComponent.h"
#include "Components/Enemies/UggWrongway/UggWrongwayRenderComponent.h"

// Audio System
#include "Sound/ServiceLocator.h"
#include "Sound/SDLSoundSystem.h"

// Achievements / Observers
#include "Observer/SteamAchievementObserver.h"


namespace fs = std::filesystem;

static void load([[maybe_unused]] bool steamInitialized)
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
    qbertObject->AddGameComponent<dae::TransformComponent>();
    qbertObject->AddGameComponent<qbert::GridMover>(gridComp->GetGrid(), 0, 0, 6.f);
    auto* qbertComp = qbertObject->AddGameComponent<qbert::QbertComponent>(gridComp->GetGrid());
    qbertObject->AddGameComponent<qbert::QbertRenderComponent>(qbertComp);
    
    // --- Coily ---
    auto coilyObject = std::make_unique<dae::GameObject>();
    coilyObject->AddGameComponent<dae::TransformComponent>();
    coilyObject->AddGameComponent<qbert::GridMover>(gridComp->GetGrid(), 0, 0, 8.f, -68.f);
    auto* coilyComp = coilyObject->AddGameComponent<qbert::CoilyComponent>(qbertObject.get());
    coilyObject->AddGameComponent<qbert::CoilyRenderComponent>(coilyComp);

    // --- Slick ---
    auto slickObject = std::make_unique<dae::GameObject>();
    slickObject->AddGameComponent<dae::TransformComponent>();
    slickObject->AddGameComponent<qbert::GridMover>(gridComp->GetGrid(), 0, 0, 15.f, -25.f);
    auto* slickComp = slickObject->AddGameComponent<qbert::SlickSamComponent>(gridComp->GetGrid(), qbert::SlickSamType::Slick);
    slickObject->AddGameComponent<qbert::SlickSamRenderComponent>(slickComp);

    // --- Sam ---
    auto samObject = std::make_unique<dae::GameObject>();
    samObject->AddGameComponent<dae::TransformComponent>();
    samObject->AddGameComponent<qbert::GridMover>(gridComp->GetGrid(), 0, 0, 15.f, -25.f);
    auto* samComp = samObject->AddGameComponent<qbert::SlickSamComponent>(gridComp->GetGrid(), qbert::SlickSamType::Sam);
    samObject->AddGameComponent<qbert::SlickSamRenderComponent>(samComp);
    
    // --- Wrong-Way ---
    auto wrongwayObject = std::make_unique<dae::GameObject>();
    wrongwayObject->AddGameComponent<dae::TransformComponent>();
    wrongwayObject->AddGameComponent<qbert::GridMover>(gridComp->GetGrid(), 6, 0, -10.f, -20.f);
    auto* wrongwayComp = wrongwayObject->AddGameComponent<qbert::UggWrongwayComponent>(qbert::UggWrongwayType::Wrongway);
    wrongwayObject->AddGameComponent<qbert::UggWrongwayRenderComponent>(wrongwayComp);

    // --- Ugg ---
    auto uggObject = std::make_unique<dae::GameObject>();
    uggObject->AddGameComponent<dae::TransformComponent>();
    uggObject->AddGameComponent<qbert::GridMover>(gridComp->GetGrid(), 6, 6, 10.f, -20.f);
    auto* uggComp = uggObject->AddGameComponent<qbert::UggWrongwayComponent>(qbert::UggWrongwayType::Ugg);
    uggObject->AddGameComponent<qbert::UggWrongwayRenderComponent>(uggComp);

    // --- Score + Steam Achievement ---
    auto* scoreComp = qbertObject->AddGameComponent<dae::ScoreComponent>();
    (void)scoreComp;

#ifdef USE_STEAMWORKS
    if (steamInitialized)
    {
        auto* achievement = qbertObject->AddGameComponent<dae::SteamAchievementObserver>();
        scoreComp->AddObserver(achievement);
    }
#endif

    // --- Input: keyboard ---
    auto& input = dae::InputManager::GetInstance();
    // --- Input: QEAD ---
    input.BindKeyboardCommand(SDLK_Q, dae::KeyState::Down,
        std::make_unique<qbert::JumpCommand>(qbertObject.get(), qbert::JumpDirection::UpLeft));

    input.BindKeyboardCommand(SDLK_E, dae::KeyState::Down,
        std::make_unique<qbert::JumpCommand>(qbertObject.get(), qbert::JumpDirection::UpRight));

    input.BindKeyboardCommand(SDLK_A, dae::KeyState::Down,
        std::make_unique<qbert::JumpCommand>(qbertObject.get(), qbert::JumpDirection::DownLeft));

    input.BindKeyboardCommand(SDLK_D, dae::KeyState::Down,
        std::make_unique<qbert::JumpCommand>(qbertObject.get(), qbert::JumpDirection::DownRight));

    // ---Input: Numberpad ---
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

    // --- Add to scene ---
    // grid
    scene.Add(std::move(gridObject));
    // player
    scene.Add(std::move(qbertObject));
    // coily
    scene.Add(std::move(coilyObject));
    // slick
    scene.Add(std::move(slickObject));
    // sam
    scene.Add(std::move(samObject));
    // wrongway
    scene.Add(std::move(wrongwayObject));
    // ugg
    scene.Add(std::move(uggObject));
}

int main(int, char* [])
{
#ifdef USE_STEAMWORKS
    const bool steamInitialized = SteamAPI_Init();
    if (!steamInitialized)
        SDL_Log("Steam not initialized.");
#endif

#if __EMSCRIPTEN__
    fs::path dataLocation = "";
#else
    fs::path dataLocation = "./Data/";
    if (!fs::exists(dataLocation))
        dataLocation = "../Data/";
#endif

    dae::Minigin engine(dataLocation);
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
