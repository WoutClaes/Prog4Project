#include "DamageCommand.h"
#include "Components/HealthComponent.h"

namespace dae
{
    DamageCommand::DamageCommand(HealthComponent* pHealth, int amount)
        : m_pHealth(pHealth)
        , m_Amount(amount)
    {}

    void DamageCommand::Execute()
    {
        if (m_pHealth)
            m_pHealth->TakeDamage(m_Amount);
    }
}
