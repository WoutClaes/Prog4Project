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
#include "GameObject.h"

// Game Managers
#include "GameManager.h"
#include "LevelLoader.h"
#include "GameMode.h"

// Core Components
#include "Components/TransformComponent.h"

// Audio System
#include "Sound/ServiceLocator.h"
#include "Sound/SDLSoundSystem.h"


namespace fs = std::filesystem;

static void load()
{
    dae::ServiceLocator::RegisterSoundSystem(std::make_unique<dae::SDLSoundSystem>());

    auto& gm = qbert::GameManager::GetInstance();

    gm.OnLoadLevel = [](int levelIndex, int stageIndex, qbert::GameMode mode)
        {
            auto& scene = dae::SceneManager::GetInstance().GetActiveScene();
            qbert::LevelLoader::Load(levelIndex, stageIndex, mode, scene);
        };

    qbert::LevelLoader::QueueLoadMainMenu();

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
