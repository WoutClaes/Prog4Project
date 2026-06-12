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

// Menu
#include "Components/Menu/MenuComponents.h"
#include "Commands/MenuCommands.h"

using json = nlohmann::json;

namespace qbert
{
    // General bindings
    static void BindGlobalSystemInputs()
    {
        auto& input = dae::InputManager::GetInstance();

        input.BindKeyboardCommand(SDLK_F1, dae::KeyState::Down, std::make_unique<SkipLevelCommand>());

        input.BindKeyboardCommand(SDLK_F2, dae::KeyState::Down, std::make_unique<ToggleMuteCommand>());

        input.BindKeyboardCommand(SDLK_F3, dae::KeyState::Down, std::make_unique<VolumeDownCommand>());
        input.BindKeyboardCommand(SDLK_F4, dae::KeyState::Down, std::make_unique<VolumeUpCommand>());
    }

    // Keyboard input bindings
    // wasd & numpad 1379
    static void BindKeyboardGameplay(dae::GameObject* pPlayer)
    {
        auto& input = dae::InputManager::GetInstance();

        input.BindKeyboardCommand(SDLK_A, dae::KeyState::Down, std::make_unique<qbert::JumpCommand>(pPlayer, qbert::JumpDirection::UpLeft));
        input.BindKeyboardCommand(SDLK_W, dae::KeyState::Down, std::make_unique<qbert::JumpCommand>(pPlayer, qbert::JumpDirection::UpRight));
        input.BindKeyboardCommand(SDLK_S, dae::KeyState::Down, std::make_unique<qbert::JumpCommand>(pPlayer, qbert::JumpDirection::DownLeft));
        input.BindKeyboardCommand(SDLK_D, dae::KeyState::Down, std::make_unique<qbert::JumpCommand>(pPlayer, qbert::JumpDirection::DownRight));

        input.BindKeyboardCommand(SDLK_KP_7, dae::KeyState::Down, std::make_unique<qbert::JumpCommand>(pPlayer, qbert::JumpDirection::UpLeft));
        input.BindKeyboardCommand(SDLK_KP_9, dae::KeyState::Down, std::make_unique<qbert::JumpCommand>(pPlayer, qbert::JumpDirection::UpRight));
        input.BindKeyboardCommand(SDLK_KP_1, dae::KeyState::Down, std::make_unique<qbert::JumpCommand>(pPlayer, qbert::JumpDirection::DownLeft));
        input.BindKeyboardCommand(SDLK_KP_3, dae::KeyState::Down, std::make_unique<qbert::JumpCommand>(pPlayer, qbert::JumpDirection::DownRight));
    }

    // Gamepad input bindings
    static void BindGamepadGameplay(dae::GameObject* pPlayer, unsigned int controllerIndex)
    {
        auto& input = dae::InputManager::GetInstance();

        input.BindControllerCommand(controllerIndex, dae::ControllerButton::DPadLeft, dae::KeyState::Down, std::make_unique<qbert::JumpCommand>(pPlayer, qbert::JumpDirection::UpLeft));
        input.BindControllerCommand(controllerIndex, dae::ControllerButton::DPadUp, dae::KeyState::Down, std::make_unique<qbert::JumpCommand>(pPlayer, qbert::JumpDirection::UpRight));
        input.BindControllerCommand(controllerIndex, dae::ControllerButton::DPadDown, dae::KeyState::Down, std::make_unique<qbert::JumpCommand>(pPlayer, qbert::JumpDirection::DownLeft));
        input.BindControllerCommand(controllerIndex, dae::ControllerButton::DPadRight, dae::KeyState::Down, std::make_unique<qbert::JumpCommand>(pPlayer, qbert::JumpDirection::DownRight));
    }

    // Q*bert creating helper function
    static dae::GameObject* MakeQbert(dae::Scene& scene, CubeGrid* grid, int startRow, int startCol, int playerIndex)
    {
        auto obj = std::make_unique<dae::GameObject>();
        obj->AddGameComponent<dae::TransformComponent>();
        obj->AddGameComponent<GridMover>(grid, startRow, startCol, 6.f, -20.f);
        auto* qbert = obj->AddGameComponent<QbertComponent>(grid, playerIndex);
        obj->AddGameComponent<QbertRenderComponent>(qbert, playerIndex);
        obj->AddGameComponent<dae::ScoreComponent>();

        auto* ptr = obj.get();
        scene.Add(std::move(obj));
        return ptr;
    }

    // Disk creator helper function
    static void LoadDisks(dae::Scene& scene, const nlohmann::json& stageData,
        std::vector<dae::GameObject*> qbertObj, float originX, float originY)
    {
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

                diskObj->AddGameComponent<DiskComponent>(qbertObj, row, col, topCubePos);

                qbert::GameManager::GetInstance().AddDisk(row, col, screenPos);

                scene.Add(std::move(diskObj));
            }
        }
    }

    // Main level loader
    bool LevelLoader::Load(int levelIndex, int stageIndex, GameMode mode, dae::Scene& scene)
    {
        // General bindings
        BindGlobalSystemInputs();

        // All levels completed check
        constexpr int maxLevels = 3;
        if (levelIndex > maxLevels)
        {
            SDL_Log("Levels completed");
            return false;
        }

        // Open correct level file
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

        // Base data
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

        // Stage data
        int baseColor = stageData.value("baseColorIndex", 0);
        int interColor = stageData.value("intermediateColorIndex", 2);
        int targetColor = stageData.value("targetColorIndex", 3);
        int stepsToTarget = stageData.value("stepsToTarget", 1);
        bool reverts = stageData.value("reverts", false);

        qbert::GameManager::GetInstance().m_Bonus = stageData.value("Bonus", 0);

        // Create grid
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

        // Create collision
        auto collisionObj = std::make_unique<dae::GameObject>();
        collisionObj->AddGameComponent<dae::TransformComponent>();
        auto* collision = collisionObj->AddGameComponent<CollisionSystem>();

        // Q*bert player creation
        dae::GameObject* qbert1Obj = nullptr;
        QbertComponent* qbert1 = nullptr;

        // Player 2
        dae::GameObject* qbert2Obj = nullptr;
        QbertComponent* qbert2 = nullptr;

        // Inputs
        auto& input = dae::InputManager::GetInstance();
        bool gamepad0Connected = input.IsControllerConnected(0);
        bool gamepad1Connected = input.IsControllerConnected(1);

        // Single player
        if (mode == GameMode::SinglePlayer)
        {
            qbert1Obj = MakeQbert(scene, grid, 0, 0, 0);

            BindKeyboardGameplay(qbert1Obj);
            if (gamepad0Connected)
            {
                BindGamepadGameplay(qbert1Obj, 0);
            }
        }
        // Coop
        else if (mode == GameMode::Coop)
        {
            int bottomRow = 6;
            qbert1Obj = MakeQbert(scene, grid, bottomRow, 0, 0);
            
            qbert2Obj = MakeQbert(scene, grid, bottomRow, bottomRow, 1);

            qbert2 = qbert2Obj->GetGameComponent<QbertComponent>();

            BindKeyboardGameplay(qbert2Obj);

            if (gamepad1Connected)
            {
                BindGamepadGameplay(qbert1Obj, 0);
                BindGamepadGameplay(qbert2Obj, 1);
            }
            else if (gamepad0Connected)
            {
                BindGamepadGameplay(qbert1Obj, 0);
            }
        }
        // Versus
        else if (mode == GameMode::Versus)
        {
            qbert1Obj = MakeQbert(scene, grid, 0, 0, 0);

            BindGamepadGameplay(qbert1Obj, 0);
        }

        if (!qbert1Obj)
        {
            SDL_Log("LevelLoader: Failed to create Player 1!");
            return false;
        }

        // Add the Q*bert(s)
        std::vector<dae::GameObject*> activePlayers;
        qbert1 = qbert1Obj->GetGameComponent<QbertComponent>();
        collision->AddQbert(qbert1);
        activePlayers.push_back(qbert1Obj);

        if (mode == GameMode::Coop && qbert2Obj != nullptr)
        {
            collision->AddQbert(qbert2);
            activePlayers.push_back(qbert2Obj);
        }

        // Load all the disks
        LoadDisks(scene, stageData, activePlayers, originX, originY);

        // Enemy spawning
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

        // Add collision to the scene (with everything assigned)
        scene.Add(std::move(collisionObj));

        // HUD
        auto font = dae::ResourceManager::GetInstance().LoadFont("Minecraft.ttf", 24);

        // Player titles sprite
        auto player1Obj = std::make_unique<dae::GameObject>();
        player1Obj->AddGameComponent<dae::TransformComponent>()->SetPosition(40.f, 30.f);
        auto* playerSprite = player1Obj->AddGameComponent<dae::SpriteComponent>();
        playerSprite->SetSpriteSheet("Player Titles.png");
        playerSprite->SetSourceRect(0.f, 0.f, 80.f, 11.f);
        playerSprite->SetDestSize(160.f, 32.f);

        // Score
        auto scoreObj = std::make_unique<dae::GameObject>();
        scoreObj->AddGameComponent<dae::TransformComponent>()->SetPosition(40.f, 60.f);
        auto* scoreText = scoreObj->AddGameComponent<dae::TextComponent>("0", font, SDL_Color{ 255, 165, 0, 255 });

        // Change to text
        auto changeToObj = std::make_unique<dae::GameObject>();
        changeToObj->AddGameComponent<dae::TransformComponent>()->SetPosition(40.f, 120.f);
        changeToObj->AddGameComponent<dae::TextComponent>("CHANGE TO:", font, SDL_Color{ 255, 0, 0, 255 });

        // Change to block sprite
        auto targetBlockObj = std::make_unique<dae::GameObject>();
        targetBlockObj->AddGameComponent<dae::TransformComponent>()->SetPosition(70.f, 150.f);
        auto* targetBlockSprite = targetBlockObj->AddGameComponent<dae::SpriteComponent>();
        targetBlockSprite->SetSpriteSheet("Qbert Cubes.png");

        float blockSpriteWidth = 32.f;
        float blockSpriteHeight = 32.f;
        int hudTilesPerRow = 6;

        int hudRow = targetColor / hudTilesPerRow;
        int hudCol = targetColor % hudTilesPerRow;

        targetBlockSprite->SetSourceRect(
            hudCol* blockSpriteWidth,
            hudRow* blockSpriteHeight,
            blockSpriteWidth,
            blockSpriteHeight
        );
        targetBlockSprite->SetDestSize(34.f, 34.f);

        // Level text
        auto levelTextObj = std::make_unique<dae::GameObject>();
        levelTextObj->AddGameComponent<dae::TransformComponent>()->SetPosition(700.f, 30.f);
        auto* levelText = levelTextObj->AddGameComponent<dae::TextComponent>("LEVEL: 1", font, SDL_Color{ 0, 255, 0, 255 });

        // Stage text
        auto livesObj = std::make_unique<dae::GameObject>();
        livesObj->AddGameComponent<dae::TransformComponent>()->SetPosition(700.f, 60.f);
        auto* livesText = livesObj->AddGameComponent<dae::TextComponent>("ROUND: 1", font, SDL_Color{ 255, 0, 255, 255 });

        // Game over sprite already preloaded
        auto gameOverObj = std::make_unique<dae::GameObject>();
        gameOverObj->AddGameComponent<dae::TransformComponent>()->SetPosition(250.f, 200.f);
        auto* gameOverSprite = gameOverObj->AddGameComponent<dae::SpriteComponent>();

        // Game win sprite already preloaded
        auto winObj = std::make_unique<dae::GameObject>();
        winObj->AddGameComponent<dae::TransformComponent>()->SetPosition(250.f, 200.f);
        auto* winSprite = winObj->AddGameComponent<dae::SpriteComponent>();

        // Stage bonus text
        auto bonusObj = std::make_unique<dae::GameObject>();
        bonusObj->AddGameComponent<dae::TransformComponent>()->SetPosition(325.f, 425.f);
        auto* bonusText = bonusObj->AddGameComponent<dae::TextComponent>("", font, SDL_Color{ 255, 165, 0, 255 });

        // Creating the actual HUD
        auto hudObj = std::make_unique<dae::GameObject>();
        hudObj->AddGameComponent<dae::TransformComponent>();
        hudObj->AddGameComponent<HUDComponent>(scoreText, livesText, levelText, gameOverSprite, winSprite, bonusText);

        // Add all of it to the scene
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

        // Game manager updater
        auto gmObj = std::make_unique<dae::GameObject>();
        gmObj->AddGameComponent<dae::TransformComponent>();
        gmObj->AddGameComponent<GameManagerUpdater>();
        scene.Add(std::move(gmObj));

        return true;
    }

    // Helper to load the next level
    void LevelLoader::QueueLoadLevel(int levelIndex, int stageIndex, GameMode mode)
    {
        dae::SceneManager::GetInstance().QueueAction([=]() {
            dae::SceneManager::GetInstance().RemoveAllScenes();
            dae::InputManager::GetInstance().RemoveAllBindings();
            auto& scene = dae::SceneManager::GetInstance().CreateScene();

            Load(levelIndex, stageIndex, mode, scene);
            });
    }

    // Load the name input
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

    // Load game over / game win
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

    // Load the highscores
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

    // Main menu loader
    void LevelLoader::QueueLoadMainMenu()
    {
        dae::SceneManager::GetInstance().QueueAction([]() {
            dae::SceneManager::GetInstance().RemoveAllScenes();
            dae::InputManager::GetInstance().RemoveAllBindings();
            auto& scene = dae::SceneManager::GetInstance().CreateScene();

            auto font = dae::ResourceManager::GetInstance().LoadFont("Minecraft.ttf", 24);

            auto titleObj = std::make_unique<dae::GameObject>();
            titleObj->AddGameComponent<dae::TransformComponent>()->SetPosition(200.f, 100.f);
            auto* titleSprite = titleObj->AddGameComponent<dae::SpriteComponent>();
            titleSprite->SetSpriteSheet("Game Title.png");
            titleSprite->SetSourceRect(0.f, 0.f, 474.f, 150.f);
            titleSprite->SetDestSize(400.f, 120.f);
            scene.Add(std::move(titleObj));

            auto spObj = std::make_unique<dae::GameObject>();
            spObj->AddGameComponent<dae::TransformComponent>()->SetPosition(300.f, 300.f);
            spObj->AddGameComponent<dae::TextComponent>("SINGLE PLAYER", font, SDL_Color{ 255, 255, 0, 255 });
            scene.Add(std::move(spObj));

            auto coopObj = std::make_unique<dae::GameObject>();
            coopObj->AddGameComponent<dae::TransformComponent>()->SetPosition(300.f, 350.f);
            coopObj->AddGameComponent<dae::TextComponent>("CO-OP", font, SDL_Color{ 255, 255, 0, 255 });
            scene.Add(std::move(coopObj));

            auto versusObj = std::make_unique<dae::GameObject>();
            versusObj->AddGameComponent<dae::TransformComponent>()->SetPosition(300.f, 400.f);
            versusObj->AddGameComponent<dae::TextComponent>("VERSUS", font, SDL_Color{ 255, 255, 0, 255 });
            scene.Add(std::move(versusObj));

            auto arrowObj = std::make_unique<dae::GameObject>();
            auto* arrowTransform = arrowObj->AddGameComponent<dae::TransformComponent>();
            arrowTransform->SetPosition(250.f, 300.f);

            auto* arrowSprite = arrowObj->AddGameComponent<dae::SpriteComponent>();
            arrowSprite->SetSpriteSheet("Selection Arrow.png");
            arrowSprite->SetSourceRect(0.f, 0.f, 6.f, 9.f);
            arrowSprite->SetDestSize(24.f, 24.f);

            auto* selector = arrowObj->AddGameComponent<MenuSelectorComponent>();
            selector->AddOption({ 250.f, 300.f, 0.f }, []() { LevelLoader::QueueLoadControlsScreen(GameMode::SinglePlayer); });
            selector->AddOption({ 250.f, 350.f, 0.f }, []() { LevelLoader::QueueLoadControlsScreen(GameMode::Coop); });
            selector->AddOption({ 250.f, 400.f, 0.f }, []() { LevelLoader::QueueLoadControlsScreen(GameMode::Versus); });

            auto& input = dae::InputManager::GetInstance();
            // Up
            input.BindKeyboardCommand(SDLK_W, dae::KeyState::Down, std::make_unique<qbert::MenuMoveCommand>(selector, -1));
            input.BindKeyboardCommand(SDLK_KP_8, dae::KeyState::Down, std::make_unique<qbert::MenuMoveCommand>(selector, -1));

            // Down
            input.BindKeyboardCommand(SDLK_S, dae::KeyState::Down, std::make_unique<qbert::MenuMoveCommand>(selector, 1));
            input.BindKeyboardCommand(SDLK_KP_2, dae::KeyState::Down, std::make_unique<qbert::MenuMoveCommand>(selector, 1));

            // Select
            input.BindKeyboardCommand(SDLK_RETURN, dae::KeyState::Down, std::make_unique<qbert::MenuSelectCommand>(selector));

            // Up
            input.BindControllerCommand(0, dae::ControllerButton::DPadUp, dae::KeyState::Down, std::make_unique<qbert::MenuMoveCommand>(selector, -1));

            // Down
            input.BindControllerCommand(0, dae::ControllerButton::DPadDown, dae::KeyState::Down, std::make_unique<qbert::MenuMoveCommand>(selector, 1));

            // Select
            input.BindControllerCommand(0, dae::ControllerButton::ButtonA, dae::KeyState::Down, std::make_unique<qbert::MenuSelectCommand>(selector));

            // Mute
            input.BindKeyboardCommand(SDLK_F2, dae::KeyState::Down, std::make_unique<ToggleMuteCommand>());

            scene.Add(std::move(arrowObj));
            });
    }

    // Control screen
    void LevelLoader::QueueLoadControlsScreen(GameMode mode)
    {
        dae::SceneManager::GetInstance().QueueAction([mode]() {
            dae::SceneManager::GetInstance().RemoveAllScenes();
            auto& scene = dae::SceneManager::GetInstance().CreateScene();

            auto controlsObj = std::make_unique<dae::GameObject>();

            // Only player 1
            if (mode == GameMode::SinglePlayer)
            {
                controlsObj->AddGameComponent<dae::TransformComponent>()->SetPosition(150.f, 100.f);
                auto* sprite = controlsObj->AddGameComponent<dae::SpriteComponent>();
                sprite->SetSpriteSheet("P1 Controls.png");
                sprite->SetSourceRect(0.f, 0.f, 300.f, 200.f);
                sprite->SetDestSize(500.f, 300.f);
            }
            // Player 1 and 2
            else if (mode == GameMode::Coop)
            {
                controlsObj->AddGameComponent<dae::TransformComponent>()->SetPosition(100.f, 150.f);
                auto* sprite = controlsObj->AddGameComponent<dae::SpriteComponent>();
                sprite->SetSpriteSheet("P1 Controls.png");
                sprite->SetSourceRect(0.f, 0.f, 300.f, 200.f);
                sprite->SetDestSize(250.f, 150.f);

                auto controlsObj2 = std::make_unique<dae::GameObject>();
                controlsObj2->AddGameComponent<dae::TransformComponent>()->SetPosition(450.f, 150.f);
                auto* sprite2 = controlsObj2->AddGameComponent<dae::SpriteComponent>();
                sprite2->SetSpriteSheet("P2 QBert Controls.png");
                sprite2->SetSourceRect(0.f, 0.f, 300.f, 200.f);
                sprite2->SetDestSize(250.f, 150.f);

                scene.Add(std::move(controlsObj2));
            }
            // Player 1 and Coily
            else if (mode == GameMode::Versus)
            {
                controlsObj->AddGameComponent<dae::TransformComponent>()->SetPosition(100.f, 150.f);
                auto* sprite = controlsObj->AddGameComponent<dae::SpriteComponent>();
                sprite->SetSpriteSheet("P1 Controls.png");
                sprite->SetSourceRect(0.f, 0.f, 300.f, 200.f);
                sprite->SetDestSize(250.f, 150.f);

                auto controlsObj2 = std::make_unique<dae::GameObject>();
                controlsObj2->AddGameComponent<dae::TransformComponent>()->SetPosition(450.f, 150.f);
                auto* sprite2 = controlsObj2->AddGameComponent<dae::SpriteComponent>();
                sprite2->SetSpriteSheet("P2 Coily Controls.png");
                sprite2->SetSourceRect(0.f, 0.f, 300.f, 200.f);
                sprite2->SetDestSize(250.f, 150.f);

                scene.Add(std::move(controlsObj2));
            }

            scene.Add(std::move(controlsObj));

            // Bottom text
            auto font = dae::ResourceManager::GetInstance().LoadFont("Minecraft.ttf", 24);
            auto textObj = std::make_unique<dae::GameObject>();
            textObj->AddGameComponent<dae::TransformComponent>()->SetPosition(200.f, 420.f);
            textObj->AddGameComponent<dae::TextComponent>("PRESS ENTER OR 'A' TO START", font, SDL_Color{ 255, 255, 0, 255 });
            scene.Add(std::move(textObj));

            auto& input = dae::InputManager::GetInstance();
            input.BindKeyboardCommand(SDLK_RETURN, dae::KeyState::Down, std::make_unique<StartGameCommand>(mode, 1));
            input.BindControllerCommand(0, dae::ControllerButton::ButtonA, dae::KeyState::Down, std::make_unique<StartGameCommand>(mode, 1));
            });
    }

    // Level transfer
    void LevelLoader::QueueLoadLevelTransition(int levelIndex, int stageIndex, GameMode mode)
    {
        dae::SceneManager::GetInstance().QueueAction([levelIndex, stageIndex, mode]() {
            dae::SceneManager::GetInstance().RemoveAllScenes();
            auto& scene = dae::SceneManager::GetInstance().CreateScene();

            auto titleObj = std::make_unique<dae::GameObject>();
            titleObj->AddGameComponent<dae::TransformComponent>()->SetPosition(200.f, 200.f);

            auto* sprite = titleObj->AddGameComponent<dae::SpriteComponent>();

            int clampedLevel = std::clamp(levelIndex, 1, 3);

            std::string spriteName = "Level 0" + std::to_string(clampedLevel) + " Title.png";
            sprite->SetSpriteSheet(spriteName);
            sprite->SetSourceRect(0.f, 0.f, 500.f, 230.f);
            sprite->SetDestSize(400.f, 150.f);

            dae::ServiceLocator::GetSoundSystem().Play(dae::ServiceLocator::GetSoundSystem().RegisterSound("Data/Sounds/Level Screen Tune.wav"), 1.f);

            titleObj->AddGameComponent<ScreenTimerComponent>(2.0f, [levelIndex, stageIndex, mode]() {
                LevelLoader::QueueLoadLevel(levelIndex, stageIndex, mode);
                });

            scene.Add(std::move(titleObj));
            });
    }

    // Helper to get the correct level file
    std::string LevelLoader::LevelPath(int levelIndex)
    {
        std::ostringstream ss;
        ss << "Data/Levels/level_" << std::setw(2) << std::setfill('0') << levelIndex << ".json";
        return ss.str();
    }
}
