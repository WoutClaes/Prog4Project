#pragma once
#include "Command.h"

namespace dae
{
    class HealthComponent;

    class DamageCommand final : public Command
    {
    public:
        explicit DamageCommand(HealthComponent* pHealth, int amount = 1);
        void Execute() override;

    private:
        HealthComponent* m_pHealth{};
        int m_Amount{};
    };
}
