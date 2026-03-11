#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Components/TextComponent.h"
#include "Scene.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Components/FPSComponent.h"

#include "InputManager.h"
#include "Commands/MoveCommand.h"
#include "GameObject.h"
#include <glm/glm.hpp>

#include <filesystem>
namespace fs = std::filesystem;

static void load()
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene();

    // Background image
    auto go = std::make_unique<dae::GameObject>();
    go->AddGameComponent<dae::TransformComponent>();
    go->AddGameComponent<dae::RenderComponent>()->SetTexture("background.png");
    scene.Add(std::move(go));

    // DAE logo
    go = std::make_unique<dae::GameObject>();
    go->AddGameComponent<dae::TransformComponent>()->SetPosition(358.f, 180.f);
    go->AddGameComponent<dae::RenderComponent>()->SetTexture("logo.png");
    scene.Add(std::move(go));

    // Header text
    auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
    go = std::make_unique<dae::GameObject>();
    go->AddGameComponent<dae::TransformComponent>()->SetPosition(292.f, 20.f);
    go->AddGameComponent<dae::TextComponent>("Programming 4 Assignment", font, SDL_Color{ 255, 255, 255, 255 });
    scene.Add(std::move(go));

    // FPS component
    auto fpsFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);
    go = std::make_unique<dae::GameObject>();
    go->AddGameComponent<dae::TransformComponent>()->SetPosition(5.f, 5.f);
    go->AddGameComponent<dae::TextComponent>("", fpsFont, SDL_Color{ 255, 255, 255, 255 });
    go->AddGameComponent<dae::FPSComponent>();
    scene.Add(std::move(go));

    // Character 1 (WASD)
    auto go1 = std::make_unique<dae::GameObject>();
    go1->AddGameComponent<dae::TransformComponent>()->SetPosition(200.f, 288.f);
    go1->AddGameComponent<dae::RenderComponent>()->SetTexture("qBert.png"); // swap for your character texture
    dae::GameObject* pChar1 = go1.get(); // raw ptr for binding – lifetime owned by scene
    scene.Add(std::move(go1));

    constexpr float speedChar1 = 100.f; // units per second

    auto& input = dae::InputManager::GetInstance();

    // KeyState::Pressed = fires every frame while the key is held down
    input.BindKeyboardCommand(SDLK_W, dae::KeyState::Pressed,
        std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{ 0.f, -1.f, 0.f }, speedChar1));
    input.BindKeyboardCommand(SDLK_S, dae::KeyState::Pressed,
        std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{ 0.f,  1.f, 0.f }, speedChar1));
    input.BindKeyboardCommand(SDLK_A, dae::KeyState::Pressed,
        std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{ -1.f,  0.f, 0.f }, speedChar1));
    input.BindKeyboardCommand(SDLK_D, dae::KeyState::Pressed,
        std::make_unique<dae::MoveCommand>(pChar1, glm::vec3{ 1.f,  0.f, 0.f }, speedChar1));

    // ── Character 2 (DPad, double speed) ─────────────────────────────────────
    auto go2 = std::make_unique<dae::GameObject>();
    go2->AddGameComponent<dae::TransformComponent>()->SetPosition(600.f, 288.f);
    go2->AddGameComponent<dae::RenderComponent>()->SetTexture("qBert.png"); // swap for your character texture
    dae::GameObject* pChar2 = go2.get();
    scene.Add(std::move(go2));

    constexpr float speedChar2 = speedChar1 * 2.f;

    input.BindControllerCommand(0, dae::ControllerButton::DPadUp, dae::KeyState::Pressed,
        std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{ 0.f, -1.f, 0.f }, speedChar2));
    input.BindControllerCommand(0, dae::ControllerButton::DPadDown, dae::KeyState::Pressed,
        std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{ 0.f,  1.f, 0.f }, speedChar2));
    input.BindControllerCommand(0, dae::ControllerButton::DPadLeft, dae::KeyState::Pressed,
        std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{ -1.f,  0.f, 0.f }, speedChar2));
    input.BindControllerCommand(0, dae::ControllerButton::DPadRight, dae::KeyState::Pressed,
        std::make_unique<dae::MoveCommand>(pChar2, glm::vec3{ 1.f,  0.f, 0.f }, speedChar2));
}

int main(int, char* []) {
#if __EMSCRIPTEN__
    fs::path data_location = "";
#else
    fs::path data_location = "./Data/";
    if (!fs::exists(data_location))
        data_location = "../Data/";
#endif
    dae::Minigin engine(data_location);
    engine.Run(load);
    return 0;
}