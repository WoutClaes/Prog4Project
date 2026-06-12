#pragma once
#include "Components/GameComponent.h"
#include "GameMode.h"
#include <string>
#include <vector>

namespace dae { class GameObject; }

namespace qbert
{
    class CubeGrid;
    class CollisionSystem;

    class EnemySpawnerComponent final : public dae::GameComponent
    {
    public:
        explicit EnemySpawnerComponent(dae::GameObject* pOwner, CubeGrid* pGrid, CollisionSystem* pCollision, dae::GameObject* pQbertObj, GameMode mode);
        ~EnemySpawnerComponent() override = default;

        void Update() override;
        void FixedUpdate() override {}
        void Render() const override {}

        void AddEnemyToQueue(const std::string& type);

        EnemySpawnerComponent(const EnemySpawnerComponent&) = delete;
        EnemySpawnerComponent& operator=(const EnemySpawnerComponent&) = delete;
        EnemySpawnerComponent(EnemySpawnerComponent&&) = delete;
        EnemySpawnerComponent& operator=(EnemySpawnerComponent&&) = delete;

    private:
        void SpawnNextEnemy();

        CubeGrid* m_pGrid{ nullptr };
        CollisionSystem* m_pCollision{ nullptr };
        dae::GameObject* m_pQbertObj{ nullptr };
        GameMode m_Mode;

        std::vector<std::string> m_EnemyQueue{};
        size_t m_CurrentQueueIndex{ 0 };

        float m_SpawnTimer{ 0.f };
        float m_InitialDelay{ 3.0f };
        float m_DelayBetweenSpawns{ 4.0f };
        bool m_HasStarted{ false };
    };
}