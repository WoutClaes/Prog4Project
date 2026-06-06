#pragma once
#include "Command.h"
#include "GameManager.h"

namespace qbert
{
    class StartGameCommand final : public dae::Command
    {
    public:
        void Execute() override
        {
            GameManager::GetInstance().StartGame(GameMode::SinglePlayer, 0);
        }
    };
}