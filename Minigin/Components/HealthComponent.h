#pragma once
#include "GameComponent.h"
#include "Observer/Subject.h"

namespace dae
{
    class HealthComponent final : public GameComponent, public Subject
    {
    public:
        explicit HealthComponent(GameObject* pOwner, int maxLives = 3);
        ~HealthComponent() override = default;

        void Update()      override {}
        void FixedUpdate() override {}
        void Render() const override {}

        void TakeDamage(int amount = 1);

        int GetLives()   const { return m_Lives; }
        int GetMaxLives() const { return m_MaxLives; }
        bool IsDead()    const { return m_Lives <= 0; }

        HealthComponent(const HealthComponent&) = delete;
        HealthComponent& operator=(const HealthComponent&) = delete;
        HealthComponent(HealthComponent&&) = delete;
        HealthComponent& operator=(HealthComponent&&) = delete;

    private:
        int m_Lives{};
        int m_MaxLives{};
    };
}
