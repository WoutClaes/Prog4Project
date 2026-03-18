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

static void load(bool steamInitialized)
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene();
    auto& input = dae::InputManager::GetInstance();

    // Shared font
    auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);

    // FPS counter
    {
        auto go = std::make_unique<dae::GameObject>();
        go->AddGameComponent<dae::TransformComponent>()->SetPosition(5.f, 5.f);
        go->AddGameComponent<dae::TextComponent>("", font);
        go->AddGameComponent<dae::FPSComponent>();
        scene.Add(std::move(go));
    }

    // Controls hint
    {
        auto hintFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 14);
        auto go = std::make_unique<dae::GameObject>();
        go->AddGameComponent<dae::TransformComponent>()->SetPosition(10.f, 30.f);
        go->AddGameComponent<dae::TextComponent>(
            "Use D-Pad to move Qbert P1, X to take damage, A/B to pick up pellets", hintFont);
        scene.Add(std::move(go));

        go = std::make_unique<dae::GameObject>();
        go->AddGameComponent<dae::TransformComponent>()->SetPosition(10.f, 48.f);
        go->AddGameComponent<dae::TextComponent>(
            "Use WASD to move Qbert P2, C to take damage, Z/X to pick up pellets", hintFont);
        scene.Add(std::move(go));
    }

    // Steam
#ifdef USE_STEAMWORKS
    {
        auto steamFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 14);
        auto go = std::make_unique<dae::GameObject>();
        go->AddGameComponent<dae::TransformComponent>()->SetPosition(5.f, 555.f);
        go->AddGameComponent<dae::TextComponent>(
            steamInitialized ? "Steam: OK" : "Steam: FAILED",
            steamFont,
            steamInitialized ? SDL_Color{ 0, 255, 0, 255 } : SDL_Color{ 255, 0, 0, 255 }
        );
        scene.Add(std::move(go));
    }

    // F1 resets all Steam achievements
    if (steamInitialized)
    {
        // Reset on startup
        dae::ResetAchievementsCommand{}.Execute();

        input.BindKeyboardCommand(SDLK_F1, dae::KeyState::Down,
            std::make_unique<dae::ResetAchievementsCommand>());
    }
#else
    (void)steamInitialized;
#endif

    // Player 1 (controller 0 – DPad movement)
    dae::HealthComponent* health1{};
    dae::ScoreComponent*  score1{};
    {
        auto go = std::make_unique<dae::GameObject>();
        go->AddGameComponent<dae::TransformComponent>()->SetPosition(200.f, 288.f);
        auto* sprite1 = go->AddGameComponent<dae::SpriteComponent>();
        sprite1->SetSpriteSheet("Qbert P1 Spritesheet.png");
        sprite1->SetSourceRect(51.f, 0.f, 17.f, 16.f);
        sprite1->SetDestSize(34.f, 32.f);
        health1 = go->AddGameComponent<dae::HealthComponent>(3);
        score1  = go->AddGameComponent<dae::ScoreComponent>();
        dae::GameObject* pChar1 = go.get();
        scene.Add(std::move(go));

        constexpr float speed1 = 100.f;
        input.BindControllerCommand(0, dae::ControllerButton::DPadUp,    dae::KeyState::Pressed,
            std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{ 0.f, -1.f, 0.f }, speed1));
        input.BindControllerCommand(0, dae::ControllerButton::DPadDown,  dae::KeyState::Pressed,
            std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{ 0.f,  1.f, 0.f }, speed1));
        input.BindControllerCommand(0, dae::ControllerButton::DPadLeft,  dae::KeyState::Pressed,
            std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{-1.f,  0.f, 0.f }, speed1));
        input.BindControllerCommand(0, dae::ControllerButton::DPadRight, dae::KeyState::Pressed,
            std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{ 1.f,  0.f, 0.f }, speed1));

        input.BindControllerCommand(0, dae::ControllerButton::ButtonX, dae::KeyState::Down,
            std::make_unique<dae::DamageCommand>(health1));
        input.BindControllerCommand(0, dae::ControllerButton::ButtonA, dae::KeyState::Down,
            std::make_unique<dae::AddPointsCommand>(score1, 10));
        input.BindControllerCommand(0, dae::ControllerButton::ButtonB, dae::KeyState::Down,
            std::make_unique<dae::AddPointsCommand>(score1, 25));
    }

    MakeLivesDisplay(scene, font, 10.f,  80.f, health1);
    MakeScoreDisplay(scene, font, 10.f, 100.f, score1);

    // Player 2 (keyboard – WASD movement)
    dae::HealthComponent* health2{};
    dae::ScoreComponent*  score2{};
    {
        auto go = std::make_unique<dae::GameObject>();
        go->AddGameComponent<dae::TransformComponent>()->SetPosition(600.f, 288.f);
        auto* sprite2 = go->AddGameComponent<dae::SpriteComponent>();
        sprite2->SetSpriteSheet("Qbert P2 Spritesheet.png");
        sprite2->SetSourceRect(51.f, 0.f, 17.f, 16.f);
        sprite2->SetDestSize(34.f, 32.f);
        health2 = go->AddGameComponent<dae::HealthComponent>(3);
        score2  = go->AddGameComponent<dae::ScoreComponent>();
        dae::GameObject* pChar2 = go.get();
        scene.Add(std::move(go));

        constexpr float speed2 = 100.f;
        input.BindKeyboardCommand(SDLK_W, dae::KeyState::Pressed,
            std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{ 0.f, -1.f, 0.f }, speed2));
        input.BindKeyboardCommand(SDLK_S, dae::KeyState::Pressed,
            std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{ 0.f,  1.f, 0.f }, speed2));
        input.BindKeyboardCommand(SDLK_A, dae::KeyState::Pressed,
            std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{-1.f,  0.f, 0.f }, speed2));
        input.BindKeyboardCommand(SDLK_D, dae::KeyState::Pressed,
            std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{ 1.f,  0.f, 0.f }, speed2));

        input.BindKeyboardCommand(SDLK_C, dae::KeyState::Down,
            std::make_unique<dae::DamageCommand>(health2));
        input.BindKeyboardCommand(SDLK_Z, dae::KeyState::Down,
            std::make_unique<dae::AddPointsCommand>(score2, 10));
        input.BindKeyboardCommand(SDLK_X, dae::KeyState::Down,
            std::make_unique<dae::AddPointsCommand>(score2, 25));
    }

    MakeLivesDisplay(scene, font, 10.f, 120.f, health2);
    MakeScoreDisplay(scene, font, 10.f, 140.f, score2);
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
