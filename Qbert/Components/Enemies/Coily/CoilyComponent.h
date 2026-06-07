#pragma once
#include "Components/GameComponent.h"
#include "Grid/CubeGrid.h"
#include "States/Coily/CoilyStates.h"
#include <memory>
#include <functional>

namespace qbert
{
    class GridMover;

    class CoilyComponent final : public dae::GameComponent
    {
    public:
        explicit CoilyComponent(dae::GameObject* pOwner, dae::GameObject* pQbertObject);
        ~CoilyComponent() override = default;

        void Update()      override;
        void FixedUpdate() override {}
        void Render() const override {}

        void Jump(JumpDirection dir);

        bool IsDead() const { return m_IsDead; }
        void Die();

        int GetRow() const;
        int GetCol() const;

        CoilyFrame GetCurrentFrame() const;
        dae::GameObject* GetQbertObject()  const { return m_pQbertObject; }

        CoilyComponent(const CoilyComponent&) = delete;
        CoilyComponent& operator=(const CoilyComponent&) = delete;
        CoilyComponent(CoilyComponent&&) = delete;
        CoilyComponent& operator=(CoilyComponent&&) = delete;

    private:
        dae::GameObject* m_pQbertObject{ nullptr };
        GridMover*       m_pMover{ nullptr };

        static constexpr int DeltaRow[4] = {  1,  1, -1, -1 };
        static constexpr int DeltaCol[4] = {  0,  1, -1,  0 };

        std::unique_ptr<ICoilyState> m_pState{};

        float m_LandPauseTimer{ 0.f };
        static constexpr float LandPause{ 0.3f };

        bool m_IsDead{ false };
    };
}
