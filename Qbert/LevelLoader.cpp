#include "LevelLoader.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <SDL3/SDL.h>

// Engine
#include "Scene.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Components/TransformComponent.h"
#include "Components/ScoreComponent.h"
#include "Observer/SteamAchievementObserver.h"

// Grid
#include "Grid/CubeGrid.h"
#include "Components/Grid/CubeGridComponent.h"
#include "Components/Grid/GridRenderComponent.h"
#include "Components/Grid/GridMover.h"

// Player
#include "Components/Player/QbertComponent.h"
#include "Components/Player/QbertRenderComponent.h"
#include "Commands/JumpCommand.h"

// Enemies
#include "Components/Enemies/Coily/CoilyComponent.h"
#include "Components/Enemies/Coily/CoilyRenderComponent.h"
#include "Components/Enemies/SlickSam/SlickSamComponent.h"
#include "Components/Enemies/SlickSam/SlickSamRenderComponent.h"
#include "Components/Enemies/UggWrongway/UggWrongwayComponent.h"
#include "Components/Enemies/UggWrongway/UggWrongwayRenderComponent.h"

// Collision + Game
#include "Components/Collision/CollisionSystem.h"
#include "GameManager.h"
#include "GameManagerUpdater.h"

using json = nlohmann::json;

namespace qbert
{
    static QbertComponent* MakeQbert(dae::Scene& scene, CubeGrid* grid,
        int controllerIndex, bool useKeyboard)
    {
        auto obj = std::make_unique<dae::GameObject>();
        obj->AddGameComponent<dae::TransformComponent>();
        obj->AddGameComponent<GridMover>(grid, 0, 0, 6.f, -20.f);
        auto* qbert = obj->AddGameComponent<QbertComponent>(grid);
        obj->AddGameComponent<QbertRenderComponent>(qbert);
        obj->AddGameComponent<dae::ScoreComponent>();

        auto& input = dae::InputManager::GetInstance();

        if (useKeyboard)
        {
            input.BindKeyboardCommand(SDLK_Q, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::UpLeft));
            input.BindKeyboardCommand(SDLK_E, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::UpRight));
            input.BindKeyboardCommand(SDLK_A, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::DownLeft));
            input.BindKeyboardCommand(SDLK_D, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::DownRight));
            input.BindKeyboardCommand(SDLK_KP_7, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::UpLeft));
            input.BindKeyboardCommand(SDLK_KP_9, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::UpRight));
            input.BindKeyboardCommand(SDLK_KP_1, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::DownLeft));
            input.BindKeyboardCommand(SDLK_KP_3, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::DownRight));
        }

        if (controllerIndex >= 0)
        {
            const unsigned int ci = static_cast<unsigned int>(controllerIndex);
            input.BindControllerCommand(ci, dae::ControllerButton::DPadLeft, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::UpLeft));
            input.BindControllerCommand(ci, dae::ControllerButton::DPadRight, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::DownRight));
            input.BindControllerCommand(ci, dae::ControllerButton::DPadUp, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::UpRight));
            input.BindControllerCommand(ci, dae::ControllerButton::DPadDown, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::DownLeft));
        }

        auto* ptr = qbert;
        scene.Add(std::move(obj));
        return ptr;
    }

    static CoilyComponent* MakeCoily(dae::Scene& scene, CubeGrid* grid,
        dae::GameObject* qbertObject)
    {
        auto obj = std::make_unique<dae::GameObject>();
        obj->AddGameComponent<dae::TransformComponent>();
        obj->AddGameComponent<GridMover>(grid, 0, 0, 8.f, -68.f);
        auto* coily = obj->AddGameComponent<CoilyComponent>(qbertObject);
        obj->AddGameComponent<CoilyRenderComponent>(coily);
        auto* ptr = coily;
        scene.Add(std::move(obj));
        return ptr;
    }

    static std::pair<SlickSamComponent*, SlickSamComponent*>
        MakeSlickSam(dae::Scene& scene, CubeGrid* grid)
    {
        auto slick = std::make_unique<dae::GameObject>();
        slick->AddGameComponent<dae::TransformComponent>();
        slick->AddGameComponent<GridMover>(grid, 0, 0, 15.f, -25.f);
        auto* sc = slick->AddGameComponent<SlickSamComponent>(grid, SlickSamType::Slick);
        slick->AddGameComponent<SlickSamRenderComponent>(sc);
        scene.Add(std::move(slick));

        auto sam = std::make_unique<dae::GameObject>();
        sam->AddGameComponent<dae::TransformComponent>();
        sam->AddGameComponent<GridMover>(grid, 0, 0, 15.f, -25.f);
        auto* samc = sam->AddGameComponent<SlickSamComponent>(grid, SlickSamType::Sam);
        sam->AddGameComponent<SlickSamRenderComponent>(samc);
        scene.Add(std::move(sam));

        return { sc, samc };
    }

    static std::pair<UggWrongwayComponent*, UggWrongwayComponent*>
        MakeUggWrongway(dae::Scene& scene, CubeGrid* grid)
    {
        auto wrongway = std::make_unique<dae::GameObject>();
        wrongway->AddGameComponent<dae::TransformComponent>();
        wrongway->AddGameComponent<GridMover>(grid, 6, 0, -10.f, -20.f);
        auto* wc = wrongway->AddGameComponent<UggWrongwayComponent>(UggWrongwayType::Wrongway);
        wrongway->AddGameComponent<UggWrongwayRenderComponent>(wc);
        scene.Add(std::move(wrongway));

        auto ugg = std::make_unique<dae::GameObject>();
        ugg->AddGameComponent<dae::TransformComponent>();
        ugg->AddGameComponent<GridMover>(grid, 6, 6, 10.f, -20.f);
        auto* uc = ugg->AddGameComponent<UggWrongwayComponent>(UggWrongwayType::Ugg);
        ugg->AddGameComponent<UggWrongwayRenderComponent>(uc);
        scene.Add(std::move(ugg));

        return { wc, uc };
    }

    bool LevelLoader::Load(int levelIndex, GameMode mode, dae::Scene& scene)
    {
        const std::string path = LevelPath(levelIndex);
        std::ifstream file(path);
        if (!file.is_open())
        {
            SDL_Log("LevelLoader: could not open %s", path.c_str());
            return false;
        }

        json data;
        try { file >> data; }
        catch (const json::parse_error& e)
        {
            SDL_Log("LevelLoader: JSON parse error in %s: %s", path.c_str(), e.what());
            return false;
        }

        const float originX = data.value("originX", 400.f);
        const float originY = data.value("originY", 50.f);
        const float scale = data.value("scale", 2.f);
        const int baseCol = data.value("baseColorIndex", 1);
        const int interCol = data.value("intermediateColorIndex", 2);
        const int targCol = data.value("targetColorIndex", 3);
        const int steps = data.value("stepsToTarget", 1);

        auto gridObj = std::make_unique<dae::GameObject>();
        gridObj->AddGameComponent<dae::TransformComponent>()->SetPosition(0.f, 0.f);
        auto* gridComp = gridObj->AddGameComponent<CubeGridComponent>(originX, originY, scale);
        gridComp->GetGrid()->SetStepsToTarget(steps);
        auto* renderComp = gridObj->AddGameComponent<GridRenderComponent>(gridComp->GetGrid());
        renderComp->SetBaseColorIndex(baseCol);
        renderComp->SetIntermediateColorIndex(interCol);
        renderComp->SetTargetColorIndex(targCol);
        CubeGrid* grid = gridComp->GetGrid();
        scene.Add(std::move(gridObj));

        auto collisionObj = std::make_unique<dae::GameObject>();
        collisionObj->AddGameComponent<dae::TransformComponent>();
        auto* collision = collisionObj->AddGameComponent<CollisionSystem>();

        QbertComponent* qbert1 = nullptr;
        dae::GameObject* qbert1Obj = nullptr;

        if (mode == GameMode::SinglePlayer)
        {
            {
                auto obj = std::make_unique<dae::GameObject>();
                obj->AddGameComponent<dae::TransformComponent>();
                obj->AddGameComponent<GridMover>(grid, 0, 0, 6.f, -20.f);
                qbert1 = obj->AddGameComponent<QbertComponent>(grid);
                obj->AddGameComponent<QbertRenderComponent>(qbert1);
                obj->AddGameComponent<dae::ScoreComponent>();

                auto& input = dae::InputManager::GetInstance();
                input.BindKeyboardCommand(SDLK_Q, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::UpLeft));
                input.BindKeyboardCommand(SDLK_E, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::UpRight));
                input.BindKeyboardCommand(SDLK_A, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::DownLeft));
                input.BindKeyboardCommand(SDLK_D, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::DownRight));
                input.BindKeyboardCommand(SDLK_KP_7, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::UpLeft));
                input.BindKeyboardCommand(SDLK_KP_9, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::UpRight));
                input.BindKeyboardCommand(SDLK_KP_1, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::DownLeft));
                input.BindKeyboardCommand(SDLK_KP_3, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::DownRight));
                input.BindControllerCommand(0, dae::ControllerButton::DPadLeft, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::UpLeft));
                input.BindControllerCommand(0, dae::ControllerButton::DPadRight, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::DownRight));
                input.BindControllerCommand(0, dae::ControllerButton::DPadUp, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::UpRight));
                input.BindControllerCommand(0, dae::ControllerButton::DPadDown, dae::KeyState::Down, std::make_unique<JumpCommand>(obj.get(), JumpDirection::DownLeft));

                qbert1Obj = obj.get();
                scene.Add(std::move(obj));
            }
        }
        else if (mode == GameMode::Coop)
        {
            qbert1 = MakeQbert(scene, grid, 0, true);
            MakeQbert(scene, grid, 1, false);
        }
        else if (mode == GameMode::Versus)
        {
            qbert1 = MakeQbert(scene, grid, 0, true);
        }

        collision->AddQbert(qbert1);

        if (data.contains("enemies"))
        {
            for (const auto& enemy : data["enemies"])
            {
                const std::string type = enemy.value("type", "");

                if (type == "Coily" && mode != GameMode::Versus)
                {
                    auto* coily = MakeCoily(scene, grid, qbert1Obj);
                    collision->AddCoily(coily);
                }
                else if (type == "SlickSam")
                {
                    auto [slick, sam] = MakeSlickSam(scene, grid);
                    collision->AddSlickSam(slick);
                    collision->AddSlickSam(sam);
                }
                else if (type == "UggWrongway")
                {
                    auto [wrongway, ugg] = MakeUggWrongway(scene, grid);
                    collision->AddUggWrongway(wrongway);
                    collision->AddUggWrongway(ugg);
                }
            }
        }

        scene.Add(std::move(collisionObj));

        auto gmObj = std::make_unique<dae::GameObject>();
        gmObj->AddGameComponent<dae::TransformComponent>();
        gmObj->AddGameComponent<GameManagerUpdater>();
        scene.Add(std::move(gmObj));

        return true;
    }

    std::string LevelLoader::LevelPath(int levelIndex)
    {
        std::ostringstream ss;
        ss << "Data/Levels/level_" << std::setw(2) << std::setfill('0') << levelIndex << ".json";
        return ss.str();
    }
}
