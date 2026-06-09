#include "EnemySpawnerComponent.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Components/TransformComponent.h"
#include "InputManager.cpp"

// Grid & Collisions
#include "Grid/CubeGrid.h"
#include "Components/Grid/GridMover.h"
#include "Components/Collision/CollisionSystem.h"

// Enemy Components & Renders
#include "Components/Enemies/Coily/CoilyComponent.h"
#include "Components/Enemies/Coily/CoilyRenderComponent.h"
#include "Commands/CoilyCommands.h"
#include "Components/Enemies/SlickSam/SlickSamComponent.h"
#include "Components/Enemies/SlickSam/SlickSamRenderComponent.h"
#include "Components/Enemies/UggWrongway/UggWrongwayComponent.h"
#include "Components/Enemies/UggWrongway/UggWrongwayRenderComponent.h"

namespace qbert
{
    EnemySpawnerComponent::EnemySpawnerComponent(dae::GameObject* pOwner, CubeGrid* pGrid,
        CollisionSystem* pCollision, dae::GameObject* pQbertObj, GameMode mode)
        : GameComponent(pOwner)
        , m_pGrid(pGrid)
        , m_pCollision(pCollision)
        , m_pQbertObj(pQbertObj)
        , m_Mode(mode)
    {}

    void EnemySpawnerComponent::AddEnemyToQueue(const std::string& type)
    {
        m_EnemyQueue.push_back(type);
    }

    void EnemySpawnerComponent::Update()
    {
        if (m_CurrentQueueIndex >= m_EnemyQueue.size()) return;

        const float dt = dae::GameTime::GetInstance().GetDeltaTime();
        m_SpawnTimer += dt;

        const float targetTime = m_HasStarted ? m_DelayBetweenSpawns : m_InitialDelay;

        if (m_SpawnTimer >= targetTime)
        {
            m_SpawnTimer = 0.f;
            m_HasStarted = true;
            SpawnNextEnemy();
        }
    }

    void EnemySpawnerComponent::SpawnNextEnemy()
    {
        if (m_CurrentQueueIndex >= m_EnemyQueue.size()) return;

        std::string type = m_EnemyQueue[m_CurrentQueueIndex];
        m_CurrentQueueIndex++;

        dae::SceneManager::GetInstance().QueueAction([this, type]()
            {
                auto& scene = dae::SceneManager::GetInstance().GetActiveScene();

                if (type == "Coily")
                {
                    auto coilyObj = std::make_unique<dae::GameObject>();
                    coilyObj->AddGameComponent<dae::TransformComponent>();

                    coilyObj->AddGameComponent<GridMover>(m_pGrid, 0, 0, 8.f, -68.f);

                    auto* coily = coilyObj->AddGameComponent<CoilyComponent>(m_pQbertObj);
                    coilyObj->AddGameComponent<CoilyRenderComponent>(coily);

                    m_pCollision->AddCoily(coily);

                    if (m_Mode == GameMode::Versus)
                    {
                        coily->m_IsPlayerControlled = true;

                        auto& input = dae::InputManager::GetInstance();
                        auto* rawCoilyPtr = coilyObj.get();

                        input.BindControllerCommand(1, dae::ControllerButton::DPadLeft, dae::KeyState::Down, std::make_unique<qbert::CoilyJumpCommand>(rawCoilyPtr, qbert::JumpDirection::UpLeft));
                        input.BindControllerCommand(1, dae::ControllerButton::DPadUp, dae::KeyState::Down, std::make_unique<qbert::CoilyJumpCommand>(rawCoilyPtr, qbert::JumpDirection::UpRight));
                        input.BindControllerCommand(1, dae::ControllerButton::DPadDown, dae::KeyState::Down, std::make_unique<qbert::CoilyJumpCommand>(rawCoilyPtr, qbert::JumpDirection::DownLeft));
                        input.BindControllerCommand(1, dae::ControllerButton::DPadRight, dae::KeyState::Down, std::make_unique<qbert::CoilyJumpCommand>(rawCoilyPtr, qbert::JumpDirection::DownRight));

                        input.BindKeyboardCommand(SDLK_Q, dae::KeyState::Down, std::make_unique<qbert::CoilyJumpCommand>(rawCoilyPtr, qbert::JumpDirection::UpLeft));
                        input.BindKeyboardCommand(SDLK_E, dae::KeyState::Down, std::make_unique<qbert::CoilyJumpCommand>(rawCoilyPtr, qbert::JumpDirection::UpRight));
                        input.BindKeyboardCommand(SDLK_A, dae::KeyState::Down, std::make_unique<qbert::CoilyJumpCommand>(rawCoilyPtr, qbert::JumpDirection::DownLeft));
                        input.BindKeyboardCommand(SDLK_D, dae::KeyState::Down, std::make_unique<qbert::CoilyJumpCommand>(rawCoilyPtr, qbert::JumpDirection::DownRight));

                        coily->OnDeathCallback = [this]() { AddEnemyToQueue("Coily");};
                    }
                    else 
                    {
                        coily->OnDeathCallback = [this]() { AddEnemyToQueue("Coily"); };
                    }

                    scene.Add(std::move(coilyObj));
                }
                else if (type == "SlickSam" || type == "Slick")
                {
                    auto slick = std::make_unique<dae::GameObject>();
                    slick->AddGameComponent<dae::TransformComponent>();
                    slick->AddGameComponent<GridMover>(m_pGrid, 0, 0, 15.f, -25.f);
                    auto* sc = slick->AddGameComponent<SlickSamComponent>(m_pGrid, SlickSamType::Slick);
                    sc->OnDeathCallback = [this]() { AddEnemyToQueue("Slick"); };
                    slick->AddGameComponent<SlickSamRenderComponent>(sc);
                    m_pCollision->AddSlickSam(sc);
                    scene.Add(std::move(slick));
                    if (type == "SlickSam")
                    {
                        m_EnemyQueue.insert(m_EnemyQueue.begin() + m_CurrentQueueIndex, "Sam");
                    }
                }
                else if (type == "Sam")
                {
                    auto sam = std::make_unique<dae::GameObject>();
                    sam->AddGameComponent<dae::TransformComponent>();
                    sam->AddGameComponent<GridMover>(m_pGrid, 0, 0, 15.f, -25.f);
                    auto* samc = sam->AddGameComponent<SlickSamComponent>(m_pGrid, SlickSamType::Sam);
                    samc->OnDeathCallback = [this]() { AddEnemyToQueue("Sam"); };
                    sam->AddGameComponent<SlickSamRenderComponent>(samc);
                    m_pCollision->AddSlickSam(samc);
                    scene.Add(std::move(sam));
                }
                else if (type == "UggWrongway" || type == "Wrongway")
                {
                    auto wrongway = std::make_unique<dae::GameObject>();
                    wrongway->AddGameComponent<dae::TransformComponent>();
                    wrongway->AddGameComponent<GridMover>(m_pGrid, 6, 0, -10.f, -20.f);
                    auto* wc = wrongway->AddGameComponent<UggWrongwayComponent>(UggWrongwayType::Wrongway);
                    wc->OnDeathCallback = [this]() { AddEnemyToQueue("Wrongway"); };
                    wrongway->AddGameComponent<UggWrongwayRenderComponent>(wc);
                    m_pCollision->AddUggWrongway(wc);
                    scene.Add(std::move(wrongway));

                    if (type == "UggWrongway") {
                        m_EnemyQueue.insert(m_EnemyQueue.begin() + m_CurrentQueueIndex, "Ugg");
                    }
                }
                else if (type == "Ugg")
                {
                    auto ugg = std::make_unique<dae::GameObject>();
                    ugg->AddGameComponent<dae::TransformComponent>();
                    ugg->AddGameComponent<GridMover>(m_pGrid, 6, 6, 10.f, -20.f);
                    auto* uc = ugg->AddGameComponent<UggWrongwayComponent>(UggWrongwayType::Ugg);
                    uc->OnDeathCallback = [this]() { AddEnemyToQueue("Ugg"); };
                    ugg->AddGameComponent<UggWrongwayRenderComponent>(uc);
                    m_pCollision->AddUggWrongway(uc);
                    scene.Add(std::move(ugg));
                }
            });
    }
}