#include "EnemySpawnerComponent.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Components/TransformComponent.h"

// Grid & Collisions
#include "Grid/CubeGrid.h"
#include "Components/Grid/GridMover.h"
#include "Components/Collision/CollisionSystem.h"

// Enemy Components & Renders
#include "Components/Enemies/Coily/CoilyComponent.h"
#include "Components/Enemies/Coily/CoilyRenderComponent.h"
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

                if (type == "Coily" && m_Mode != GameMode::Versus)
                {
                    auto obj = std::make_unique<dae::GameObject>();
                    obj->AddGameComponent<dae::TransformComponent>();
                    obj->AddGameComponent<GridMover>(m_pGrid, 0, 0, 8.f, -68.f);
                    auto* coily = obj->AddGameComponent<CoilyComponent>(m_pQbertObj);
                    obj->AddGameComponent<CoilyRenderComponent>(coily);

                    m_pCollision->AddCoily(coily);
                    scene.Add(std::move(obj));
                }
                else if (type == "SlickSam")
                {
                    auto slick = std::make_unique<dae::GameObject>();
                    slick->AddGameComponent<dae::TransformComponent>();
                    slick->AddGameComponent<GridMover>(m_pGrid, 0, 0, 15.f, -25.f);
                    auto* sc = slick->AddGameComponent<SlickSamComponent>(m_pGrid, SlickSamType::Slick);
                    slick->AddGameComponent<SlickSamRenderComponent>(sc);
                    m_pCollision->AddSlickSam(sc);
                    scene.Add(std::move(slick));

                    auto sam = std::make_unique<dae::GameObject>();
                    sam->AddGameComponent<dae::TransformComponent>();
                    sam->AddGameComponent<GridMover>(m_pGrid, 0, 0, 15.f, -25.f);
                    auto* samc = sam->AddGameComponent<SlickSamComponent>(m_pGrid, SlickSamType::Sam);
                    sam->AddGameComponent<SlickSamRenderComponent>(samc);
                    m_pCollision->AddSlickSam(samc);
                    scene.Add(std::move(sam));
                }
                else if (type == "UggWrongway")
                {
                    auto wrongway = std::make_unique<dae::GameObject>();
                    wrongway->AddGameComponent<dae::TransformComponent>();
                    wrongway->AddGameComponent<GridMover>(m_pGrid, 6, 0, -10.f, -20.f);
                    auto* wc = wrongway->AddGameComponent<UggWrongwayComponent>(UggWrongwayType::Wrongway);
                    wrongway->AddGameComponent<UggWrongwayRenderComponent>(wc);
                    m_pCollision->AddUggWrongway(wc);
                    scene.Add(std::move(wrongway));

                    auto ugg = std::make_unique<dae::GameObject>();
                    ugg->AddGameComponent<dae::TransformComponent>();
                    ugg->AddGameComponent<GridMover>(m_pGrid, 6, 6, 10.f, -20.f);
                    auto* uc = ugg->AddGameComponent<UggWrongwayComponent>(UggWrongwayType::Ugg);
                    ugg->AddGameComponent<UggWrongwayRenderComponent>(uc);
                    m_pCollision->AddUggWrongway(uc);
                    scene.Add(std::move(ugg));
                }
            });
    }
}