#pragma once
#include "Components/GameComponent.h"
#include "GameManager.h"

namespace qbert
{
    class GameManagerUpdater final : public dae::GameComponent
    {
    public:
        explicit GameManagerUpdater(dae::GameObject* pOwner)
            : GameComponent(pOwner) {}
        ~GameManagerUpdater() override = default;

        void Update()      override { GameManager::GetInstance().Update(); }
        void FixedUpdate() override {}
        void Render() const override {}

        GameManagerUpdater(const GameManagerUpdater&) = delete;
        GameManagerUpdater& operator=(const GameManagerUpdater&) = delete;
        GameManagerUpdater(GameManagerUpdater&&) = delete;
        GameManagerUpdater& operator=(GameManagerUpdater&&) = delete;
    };
}