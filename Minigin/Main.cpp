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
#include "Components/OrbitComponent.h"

#include <filesystem>
namespace fs = std::filesystem;

static void load()
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene();

    auto go = std::make_unique<dae::GameObject>();
    go->AddGameComponent<dae::TransformComponent>();
    go->AddGameComponent<dae::RenderComponent>()->SetTexture("background.png");
    scene.Add(std::move(go));

    go = std::make_unique<dae::GameObject>();
    go->AddGameComponent<dae::TransformComponent>()->SetPosition(358.f, 180.f);
    go->AddGameComponent<dae::RenderComponent>()->SetTexture("logo.png");
    scene.Add(std::move(go));

    auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
    go = std::make_unique<dae::GameObject>();
    go->AddGameComponent<dae::TransformComponent>()->SetPosition(292.f, 20.f);
    go->AddGameComponent<dae::TextComponent>("Programming 4 Assignment", font, SDL_Color{ 255, 255, 255, 255 });
    scene.Add(std::move(go));

    auto fpsFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);
    go = std::make_unique<dae::GameObject>();
    go->AddGameComponent<dae::TransformComponent>()->SetPosition(5.f, 5.f);
    go->AddGameComponent<dae::TextComponent>("", fpsFont, SDL_Color{ 255, 255, 255, 255 });
    go->AddGameComponent<dae::FPSComponent>();
    scene.Add(std::move(go));

    constexpr float cx = 400.f;
    constexpr float cy = 200.f;

    auto anchor = std::make_unique<dae::GameObject>();
    auto* anchorTransform = anchor->AddGameComponent<dae::TransformComponent>();
    anchorTransform->SetPosition(cx, cy);
    dae::GameObject* anchorPtr = anchor.get();
    scene.Add(std::move(anchor));

    auto parent = std::make_unique<dae::GameObject>();
    parent->AddGameComponent<dae::TransformComponent>();
    parent->AddGameComponent<dae::RenderComponent>()->SetTexture("qBert.png");
    parent->AddGameComponent<dae::OrbitComponent>(80.f, 2.f);
    dae::GameObject* parentPtr = parent.get();
    scene.Add(std::move(parent));
    parentPtr->SetParent(anchorPtr, false);

    auto child = std::make_unique<dae::GameObject>();
    child->AddGameComponent<dae::TransformComponent>();
    child->AddGameComponent<dae::RenderComponent>()->SetTexture("qBert.png");
    child->AddGameComponent<dae::OrbitComponent>(40.f, -3.f);
    dae::GameObject* childPtr = child.get();
    scene.Add(std::move(child));
    childPtr->SetParent(parentPtr, false);
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