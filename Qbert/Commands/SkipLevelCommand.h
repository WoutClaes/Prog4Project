#pragma once
#include "Commands/Command.h"
#include "GameManager.h" 

namespace qbert
{
    class SkipLevelCommand final : public dae::Command
    {
    public:
        void Execute() override
        {
            qbert::GameManager::GetInstance().OnLevelComplete();
        }
    };
}