#include "HealthComponent.h"
#include "GameObject.h"

namespace dae
{
    HealthComponent::HealthComponent(GameObject* pOwner, int maxLives)
        : GameComponent(pOwner)
        , m_Lives(maxLives)
        , m_MaxLives(maxLives)
    {}

    void HealthComponent::TakeDamage(int amount)
    {
        if (m_Lives <= 0) return;

        m_Lives -= amount;
        if (m_Lives < 0) m_Lives = 0;

        Notify(GetOwner(), Events::PlayerDied);
    }
}
