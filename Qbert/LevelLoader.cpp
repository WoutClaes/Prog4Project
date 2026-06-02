#include "LevelLoader.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <SDL3/SDL.h>

// Engine
#include "Scene.h"
#include "GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/ScoreComponent.h"

// Grid
#include "Grid/CubeGrid.h"
#include "Components/Grid/CubeGridComponent.h"
#include "Components/Grid/GridRenderComponent.h"
#include "Components/Grid/GridMover.h"

// Player
#include "Components/Player/QbertComponent.h"
#include "Components/Player/QbertRenderComponent.h"
#include "Commands/JumpCommand.h"
#include "Commands/SkipLevelCommand.h"
#include "Commands/SoundCommands.h"

// Collision + Game
#include "Components/Collision/CollisionSystem.h"
#include "GameManagerUpdater.h"

#include "Components/Enemies/EnemySpawnerComponent.h"

using json = nlohmann::json;

namespace qbert
{
    static void BindGlobalSystemInputs()
    {
        auto& input = dae::InputManager::GetInstance();

        input.BindKeyboardCommand(SDLK_F1, dae::KeyState::Down, std::make_unique<SkipLevelCommand>());

        input.BindKeyboardCommand(SDLK_F2, dae::KeyState::Down, std::make_unique<ToggleMuteCommand>());

        input.BindKeyboardCommand(SDLK_F3, dae::KeyState::Down, std::make_unique<VolumeDownCommand>());
        input.BindKeyboardCommand(SDLK_F4, dae::KeyState::Down, std::make_unique<VolumeUpCommand>());
    }

    static void BindQbertInputs(dae::GameObject* obj, int controllerIndex, bool useKeyboard)
    {
        auto& input = dae::InputManager::GetInstance();

        if (useKeyboard)
        {
            input.BindKeyboardCommand(SDLK_Q, dae::KeyState::Down, std::make_unique<JumpCommand>(obj, JumpDirection::UpLeft));
            input.BindKeyboardCommand(SDLK_E, dae::KeyState::Down, std::make_unique<JumpCommand>(obj, JumpDirection::UpRight));
            input.BindKeyboardCommand(SDLK_A, dae::KeyState::Down, std::make_unique<JumpCommand>(obj, JumpDirection::DownLeft));
            input.BindKeyboardCommand(SDLK_D, dae::KeyState::Down, std::make_unique<JumpCommand>(obj, JumpDirection::DownRight));
            input.BindKeyboardCommand(SDLK_KP_7, dae::KeyState::Down, std::make_unique<JumpCommand>(obj, JumpDirection::UpLeft));
            input.BindKeyboardCommand(SDLK_KP_9, dae::KeyState::Down, std::make_unique<JumpCommand>(obj, JumpDirection::UpRight));
            input.BindKeyboardCommand(SDLK_KP_1, dae::KeyState::Down, std::make_unique<JumpCommand>(obj, JumpDirection::DownLeft));
            input.BindKeyboardCommand(SDLK_KP_3, dae::KeyState::Down, std::make_unique<JumpCommand>(obj, JumpDirection::DownRight));
        }

        if (controllerIndex >= 0)
        {
            const unsigned int ci = static_cast<unsigned int>(controllerIndex);
            input.BindControllerCommand(ci, dae::ControllerButton::DPadLeft, dae::KeyState::Down, std::make_unique<JumpCommand>(obj, JumpDirection::UpLeft));
            input.BindControllerCommand(ci, dae::ControllerButton::DPadRight, dae::KeyState::Down, std::make_unique<JumpCommand>(obj, JumpDirection::DownRight));
            input.BindControllerCommand(ci, dae::ControllerButton::DPadUp, dae::KeyState::Down, std::make_unique<JumpCommand>(obj, JumpDirection::UpRight));
            input.BindControllerCommand(ci, dae::ControllerButton::DPadDown, dae::KeyState::Down, std::make_unique<JumpCommand>(obj, JumpDirection::DownLeft));
        }
    }

    static QbertComponent* MakeQbert(dae::Scene& scene, CubeGrid* grid,
        int controllerIndex, bool useKeyboard)
    {
        auto obj = std::make_unique<dae::GameObject>();
        obj->AddGameComponent<dae::TransformComponent>();
        obj->AddGameComponent<GridMover>(grid, 0, 0, 6.f, -20.f);
        auto* qbert = obj->AddGameComponent<QbertComponent>(grid);
        obj->AddGameComponent<QbertRenderComponent>(qbert);
        obj->AddGameComponent<dae::ScoreComponent>();

        BindQbertInputs(obj.get(), controllerIndex, useKeyboard);

        auto* ptr = qbert;
        scene.Add(std::move(obj));
        return ptr;
    }

    bool LevelLoader::Load(int levelIndex, int stageIndex, GameMode mode, dae::Scene& scene)
    {
        BindGlobalSystemInputs();

        constexpr int maxLevels = 3;
        if (levelIndex > maxLevels)
        {
            SDL_Log("Levels completed");
            return false;
        }

        std::ifstream file(LevelPath(levelIndex));
        if (!file.is_open())
        {
            SDL_Log("LevelLoader: Failed to open level file: %s", LevelPath(levelIndex).c_str());
            return false;
        }

        nlohmann::json data;
        try { file >> data; }
        catch (const nlohmann::json::parse_error& e)
        {
            SDL_Log("LevelLoader: JSON parse error: %s", e.what());
            return false;
        }

        float originX = data.value("originX", 400.0f);
        float originY = data.value("originY", 50.0f);
        float scale = data.value("scale", 2.0f);

        nlohmann::json stageData;
        if (data.contains("stages") && data["stages"].is_array() && !data["stages"].empty())
        {
            size_t stageIdx = static_cast<size_t>(stageIndex);
            if (stageIdx >= data["stages"].size())
            {
                SDL_Log("LevelLoader: stageIndex %d out of bounds! Clamping to last stage.", stageIndex);
                stageIdx = data["stages"].size() - 1;
            }
            stageData = data["stages"][stageIdx];
        }
        else
        {
            stageData = data;
        }

        int baseColor = stageData.value("baseColorIndex", 0);
        int interColor = stageData.value("intermediateColorIndex", 2);
        int targetColor = stageData.value("targetColorIndex", 3);
        int stepsToTarget = stageData.value("stepsToTarget", 1);

        auto gridObj = std::make_unique<dae::GameObject>();
        gridObj->AddGameComponent<dae::TransformComponent>()->SetPosition(0.f, 0.f);
        auto* gridComp = gridObj->AddGameComponent<CubeGridComponent>(originX, originY, scale);
        gridComp->GetGrid()->SetStepsToTarget(stepsToTarget);
        auto* renderComp = gridObj->AddGameComponent<GridRenderComponent>(gridComp->GetGrid());
        renderComp->SetBaseColorIndex(baseColor);
        renderComp->SetIntermediateColorIndex(interColor);
        renderComp->SetTargetColorIndex(targetColor);
        CubeGrid* grid = gridComp->GetGrid();
        scene.Add(std::move(gridObj));

        auto collisionObj = std::make_unique<dae::GameObject>();
        collisionObj->AddGameComponent<dae::TransformComponent>();
        auto* collision = collisionObj->AddGameComponent<CollisionSystem>();

        dae::GameObject* qbert1Obj = nullptr;
        QbertComponent* qbert1 = nullptr;

        if (mode == GameMode::SinglePlayer)
        {
            auto obj = std::make_unique<dae::GameObject>();
            obj->AddGameComponent<dae::TransformComponent>();
            obj->AddGameComponent<GridMover>(grid, 0, 0, 6.f, -20.f);
            qbert1 = obj->AddGameComponent<QbertComponent>(grid);
            obj->AddGameComponent<QbertRenderComponent>(qbert1);
            obj->AddGameComponent<dae::ScoreComponent>();

            BindQbertInputs(obj.get(), 0, true);

            qbert1Obj = obj.get();
            scene.Add(std::move(obj));
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

        if (qbert1Obj)
        {
            qbert1 = qbert1Obj->GetGameComponent<QbertComponent>();
            collision->AddQbert(qbert1);
        }

        auto spawnerObj = std::make_unique<dae::GameObject>();
        spawnerObj->AddGameComponent<dae::TransformComponent>();

        auto* spawner = spawnerObj->AddGameComponent<EnemySpawnerComponent>(
            grid,
            collision,
            qbert1Obj,
            mode
        );

        if (stageData.contains("enemies") && stageData["enemies"].is_array())
        {
            for (const auto& enemy : stageData["enemies"])
            {
                std::string type = enemy.value("type", "");
                if (!type.empty())
                {
                    spawner->AddEnemyToQueue(type);
                }
            }
        }
        scene.Add(std::move(spawnerObj));

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
