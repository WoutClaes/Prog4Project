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
#include "Components/SpriteComponent.h"
#include "Components/TextComponent.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Components/NameInputComponent.h"
#include "Commands/NameInputCommands.h"

// Grid + Disk
#include "Grid/CubeGrid.h"
#include "Components/Grid/CubeGridComponent.h"
#include "Components/Grid/GridRenderComponent.h"
#include "Components/Grid/GridMover.h"

#include "Components/Grid/Disk/DiskComponent.h"

// Player
#include "Components/Player/QbertComponent.h"
#include "Components/Player/QbertRenderComponent.h"
#include "Commands/JumpCommand.h"
#include "Commands/SkipLevelCommand.h"
#include "Commands/SoundCommands.h"

// Collision + Game
#include "Components/Collision/CollisionSystem.h"
#include "GameManager.h"
#include "GameManagerUpdater.h"
#include "GameMode.h"

#include "Components/Enemies/EnemySpawnerComponent.h"

#include "Components/HUD/HUDComponent.h"
#include <glm/fwd.hpp>

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

    static dae::GameObject* MakeQbert(dae::Scene& scene, CubeGrid* grid,
        int controllerIndex, bool useKeyboard)
    {
        auto obj = std::make_unique<dae::GameObject>();
        obj->AddGameComponent<dae::TransformComponent>();
        obj->AddGameComponent<GridMover>(grid, 0, 0, 6.f, -20.f);
        auto* qbert = obj->AddGameComponent<QbertComponent>(grid);
        obj->AddGameComponent<QbertRenderComponent>(qbert);
        obj->AddGameComponent<dae::ScoreComponent>();

        BindQbertInputs(obj.get(), controllerIndex, useKeyboard);

        auto* ptr = obj.get();
        scene.Add(std::move(obj));
        return ptr;
    }

    static void LoadDisks(dae::Scene& scene, const nlohmann::json& stageData,
        dae::GameObject* qbert1Obj, float originX, float originY)
    {
        if (!qbert1Obj) return;

        auto* qbertComp = qbert1Obj->GetGameComponent<QbertComponent>();
        auto* gridMover = qbert1Obj->GetGameComponent<GridMover>();
        glm::vec3 topCubePos = { originX + 16, originY - 32, 0.f };

        if (stageData.contains("disks") && stageData["disks"].is_array())
        {
            for (const auto& diskData : stageData["disks"])
            {
                int row = diskData.value("row", 5);
                int col = diskData.value("col", -1);
                float offsetX = diskData.value("offsetX", -212.f);
                float offsetY = diskData.value("offsetY", 256.f);

                auto diskObj = std::make_unique<dae::GameObject>();
                auto* transform = diskObj->AddGameComponent<dae::TransformComponent>();

                glm::vec3 screenPos = { originX + offsetX, originY + offsetY, 0.f };
                transform->SetPosition(screenPos.x, screenPos.y);

                auto* sprite = diskObj->AddGameComponent<dae::SpriteComponent>();
                sprite->SetSpriteSheet("Disk Spritesheet.png");
                sprite->SetSourceRect(0.f, 0.f, 16.f, 16.f);
                sprite->SetDestSize(32.f, 32.f);

                diskObj->AddGameComponent<DiskComponent>(qbert1Obj, gridMover, row, col, topCubePos);
                qbertComp->AddDisk(row, col, screenPos);

                scene.Add(std::move(diskObj));
            }
        }
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
        bool reverts = stageData.value("reverts", false);

        qbert::GameManager::GetInstance().m_Bonus = stageData.value("Bonus", 0);

        auto gridObj = std::make_unique<dae::GameObject>();
        gridObj->AddGameComponent<dae::TransformComponent>()->SetPosition(0.f, 0.f);
        auto* gridComp = gridObj->AddGameComponent<CubeGridComponent>(originX, originY, scale);
        gridComp->GetGrid()->SetStepsToTarget(stepsToTarget);
        gridComp->GetGrid()->SetReverts(reverts);
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
            qbert1Obj = MakeQbert(scene, grid, 0, true);
        }
        else if (mode == GameMode::Coop)
        {
            qbert1Obj = MakeQbert(scene, grid, 0, true);
            MakeQbert(scene, grid, 1, false);
        }
        else if (mode == GameMode::Versus)
        {
            qbert1Obj = MakeQbert(scene, grid, 0, true);
        }

        if (!qbert1Obj)
        {
            SDL_Log("LevelLoader: Failed to create Player 1!");
            return false;
        }

        qbert1 = qbert1Obj->GetGameComponent<QbertComponent>(); 
        collision->AddQbert(qbert1);

        LoadDisks(scene, stageData, qbert1Obj, originX, originY);

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

        auto font = dae::ResourceManager::GetInstance().LoadFont("Minecraft.ttf", 24);

        auto player1Obj = std::make_unique<dae::GameObject>();
        player1Obj->AddGameComponent<dae::TransformComponent>()->SetPosition(40.f, 30.f);
        auto* playerSprite = player1Obj->AddGameComponent<dae::SpriteComponent>();
        playerSprite->SetSpriteSheet("Player Titles.png");
        playerSprite->SetSourceRect(0.f, 0.f, 80.f, 11.f);
        playerSprite->SetDestSize(160.f, 32.f);

        auto scoreObj = std::make_unique<dae::GameObject>();
        scoreObj->AddGameComponent<dae::TransformComponent>()->SetPosition(40.f, 60.f);
        auto* scoreText = scoreObj->AddGameComponent<dae::TextComponent>("0", font, SDL_Color{ 255, 165, 0, 255 });

        auto changeToObj = std::make_unique<dae::GameObject>();
        changeToObj->AddGameComponent<dae::TransformComponent>()->SetPosition(40.f, 120.f);
        changeToObj->AddGameComponent<dae::TextComponent>("CHANGE TO:", font, SDL_Color{ 255, 0, 0, 255 });

        auto targetBlockObj = std::make_unique<dae::GameObject>();
        targetBlockObj->AddGameComponent<dae::TransformComponent>()->SetPosition(70.f, 150.f);
        auto* targetBlockSprite = targetBlockObj->AddGameComponent<dae::SpriteComponent>();
        targetBlockSprite->SetSpriteSheet("Color Icons Spritesheet.png");
        float blockSpriteWidth = 14.f;
        targetBlockSprite->SetSourceRect(0.f, 0.f, blockSpriteWidth, 12.f);
        targetBlockSprite->SetDestSize(34.f, 34.f);

        auto levelTextObj = std::make_unique<dae::GameObject>();
        levelTextObj->AddGameComponent<dae::TransformComponent>()->SetPosition(500.f, 30.f);
        auto* levelText = levelTextObj->AddGameComponent<dae::TextComponent>("LEVEL: 1", font, SDL_Color{ 0, 255, 0, 255 });

        auto livesObj = std::make_unique<dae::GameObject>();
        livesObj->AddGameComponent<dae::TransformComponent>()->SetPosition(500.f, 60.f);
        auto* livesText = livesObj->AddGameComponent<dae::TextComponent>("ROUND: 1", font, SDL_Color{ 255, 0, 255, 255 });

        auto gameOverObj = std::make_unique<dae::GameObject>();
        gameOverObj->AddGameComponent<dae::TransformComponent>()->SetPosition(250.f, 200.f);
        auto* gameOverSprite = gameOverObj->AddGameComponent<dae::SpriteComponent>();

        auto winObj = std::make_unique<dae::GameObject>();
        winObj->AddGameComponent<dae::TransformComponent>()->SetPosition(250.f, 200.f);
        auto* winSprite = winObj->AddGameComponent<dae::SpriteComponent>();

        auto bonusObj = std::make_unique<dae::GameObject>();
        bonusObj->AddGameComponent<dae::TransformComponent>()->SetPosition(250.f, 400.f);
        auto* bonusText = bonusObj->AddGameComponent<dae::TextComponent>("", font, SDL_Color{ 255, 165, 0, 255 });

        auto hudObj = std::make_unique<dae::GameObject>();
        hudObj->AddGameComponent<dae::TransformComponent>();
        hudObj->AddGameComponent<HUDComponent>(scoreText, livesText, levelText, gameOverSprite, winSprite, bonusText);

        scene.Add(std::move(player1Obj));
        scene.Add(std::move(scoreObj));
        scene.Add(std::move(changeToObj));
        scene.Add(std::move(targetBlockObj));
        scene.Add(std::move(levelTextObj));
        scene.Add(std::move(livesObj));
        scene.Add(std::move(gameOverObj));
        scene.Add(std::move(winObj));
        scene.Add(std::move(bonusObj));
        scene.Add(std::move(hudObj));

        auto gmObj = std::make_unique<dae::GameObject>();
        gmObj->AddGameComponent<dae::TransformComponent>();
        gmObj->AddGameComponent<GameManagerUpdater>();
        scene.Add(std::move(gmObj));

        return true;
    }

    void LevelLoader::QueueLoadLevel(int levelIndex, int stageIndex, GameMode mode)
    {
        dae::SceneManager::GetInstance().QueueAction([=]() {
            dae::SceneManager::GetInstance().RemoveAllScenes();
            auto& scene = dae::SceneManager::GetInstance().CreateScene();

            Load(levelIndex, stageIndex, mode, scene);
            });
    }

    void LevelLoader::QueueLoadHighScoreInput(int finalScore)
    {
        dae::SceneManager::GetInstance().QueueAction([finalScore]() {
            dae::SceneManager::GetInstance().RemoveAllScenes();
            auto& scene = dae::SceneManager::GetInstance().CreateScene();

            auto inputObj = std::make_unique<dae::GameObject>();
            inputObj->AddGameComponent<dae::TransformComponent>()->SetPosition(400.f, 300.f);

            auto font = dae::ResourceManager::GetInstance().LoadFont("Minecraft.ttf", 36);
            auto* textComp = inputObj->AddGameComponent<dae::TextComponent>("AAA", font, SDL_Color{ 255, 255, 0, 255 });

            auto* nameComp = inputObj->AddGameComponent<dae::NameInputComponent>(textComp, finalScore);

            nameComp->SetOnCompleteCallback([]() {
                qbert::LevelLoader::QueueLoadHighScoreBoard();
                });

            auto& input = dae::InputManager::GetInstance();

            input.BindKeyboardCommand(SDLK_UP, dae::KeyState::Down, std::make_unique<dae::ChangeLetterCommand>(nameComp, 1));
            input.BindKeyboardCommand(SDLK_DOWN, dae::KeyState::Down, std::make_unique<dae::ChangeLetterCommand>(nameComp, -1));
            input.BindKeyboardCommand(SDLK_RETURN, dae::KeyState::Down, std::make_unique<dae::NextSlotCommand>(nameComp));

            input.BindControllerCommand(0, dae::ControllerButton::DPadUp, dae::KeyState::Down, std::make_unique<dae::ChangeLetterCommand>(nameComp, 1));
            input.BindControllerCommand(0, dae::ControllerButton::DPadDown, dae::KeyState::Down, std::make_unique<dae::ChangeLetterCommand>(nameComp, -1));
            input.BindControllerCommand(0, dae::ControllerButton::ButtonA, dae::KeyState::Down, std::make_unique<dae::NextSlotCommand>(nameComp));

            scene.Add(std::move(inputObj));
            });
    }

    void LevelLoader::QueueLoadEndScreen(bool isWin)
    {
        dae::SceneManager::GetInstance().QueueAction([isWin]() {
            dae::SceneManager::GetInstance().RemoveAllScenes();
            auto& scene = dae::SceneManager::GetInstance().CreateScene();

            auto endObj = std::make_unique<dae::GameObject>();
            endObj->AddGameComponent<dae::TransformComponent>()->SetPosition(200.f, 200.f);
            auto* sprite = endObj->AddGameComponent<dae::SpriteComponent>();

            sprite->SetSpriteSheet(isWin ? "Victory Title.png" : "Game Over Title.png");
            sprite->SetSourceRect(0.f, 0.f, isWin ? 270.f : 376, 149.f);
            sprite->SetDestSize(isWin ? 270.f : 376, 149.f);
            scene.Add(std::move(endObj));

            auto gmObj = std::make_unique<dae::GameObject>();
            gmObj->AddGameComponent<dae::TransformComponent>();
            gmObj->AddGameComponent<GameManagerUpdater>();
            scene.Add(std::move(gmObj));
            });
    }

    void LevelLoader::QueueLoadHighScoreBoard()
    {
        dae::SceneManager::GetInstance().QueueAction([]() {
            dae::SceneManager::GetInstance().RemoveAllScenes();
            auto& scene = dae::SceneManager::GetInstance().CreateScene();

            auto font = dae::ResourceManager::GetInstance().LoadFont("Minecraft.ttf", 24);
            auto scores = dae::ScoreComponent::LoadHighScores();

            auto titleObj = std::make_unique<dae::GameObject>();
            titleObj->AddGameComponent<dae::TransformComponent>()->SetPosition(300.f, 50.f);
            titleObj->AddGameComponent<dae::TextComponent>("HIGH SCORES", font, SDL_Color{ 255, 255, 255, 255 });
            scene.Add(std::move(titleObj));

            float startY = 100.f;
            for (size_t i = 0; i < scores.size(); ++i)
            {
                auto scoreObj = std::make_unique<dae::GameObject>();
                scoreObj->AddGameComponent<dae::TransformComponent>()->SetPosition(250.f, startY + (i * 30.f));

                std::string entry = std::to_string(i + 1) + ". " + scores[i].name + " - " + std::to_string(scores[i].score);
                scoreObj->AddGameComponent<dae::TextComponent>(entry, font, SDL_Color{ 255, 255, 0, 255 });

                scene.Add(std::move(scoreObj));
            }
            });
    }

    std::string LevelLoader::LevelPath(int levelIndex)
    {
        std::ostringstream ss;
        ss << "Data/Levels/level_" << std::setw(2) << std::setfill('0') << levelIndex << ".json";
        return ss.str();
    }
}
