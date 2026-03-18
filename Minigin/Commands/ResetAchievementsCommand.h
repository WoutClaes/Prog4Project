#pragma once
#include "Command.h"

namespace dae
{
    class ResetAchievementsCommand final : public Command
    {
    public:
        void Execute() override;
    };
}
