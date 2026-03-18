#pragma once
#include "Command.h"

namespace dae
{
    class ScoreComponent;

    class AddPointsCommand final : public Command
    {
    public:
        explicit AddPointsCommand(ScoreComponent* pScore, int amount);
        void Execute() override;

    private:
        ScoreComponent* m_pScore{};
        int m_Amount{};
    };
}
