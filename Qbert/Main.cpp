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

// Game Managers
#include "GameManager.h"
#include "LevelLoader.h"
#include "GameMode.h"

// Core Components
#include "Components/TransformComponent.h"
#include "Components/ScoreComponent.h"

// Grid System
#include "Grid/CubeGrid.h"
#include "Components/Grid/CubeGridComponent.h"
#include "Components/Grid/GridRenderComponent.h"
#include "Components/Grid/GridMover.h"

// Collision
#include "Components/Collision/CollisionSystem.h"

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

static void load()
{
    dae::ServiceLocator::RegisterSoundSystem(std::make_unique<dae::SDLSoundSystem>());

    auto& gm = qbert::GameManager::GetInstance();

    gm.OnLoadLevel = [](int levelIndex, qbert::GameMode mode)
        {
            auto& scene = dae::SceneManager::GetInstance().GetActiveScene();
            qbert::LevelLoader::Load(levelIndex, mode, scene);
        };

    gm.StartGame(qbert::GameMode::SinglePlayer, 0);

    auto collisionObj = std::make_unique<dae::GameObject>();
    collisionObj->AddGameComponent<dae::TransformComponent>();
}

int main(int, char* [])
{
#if __EMSCRIPTEN__
    fs::path dataLocation = "";
#else
    fs::path dataLocation = "./Data/";
    if (!fs::exists(dataLocation))
        dataLocation = "../Data/";
#endif

    dae::Minigin engine(dataLocation);

    engine.Run([]() { load(); });

    return 0;
}
