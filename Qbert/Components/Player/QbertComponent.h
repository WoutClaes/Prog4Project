#pragma once
#include "Components/GameComponent.h"
#include "Grid/CubeGrid.h"
#include <functional>

namespace qbert
{
    class GridMover;

    class QbertComponent final : public dae::GameComponent
    {
    public:
        explicit QbertComponent(dae::GameObject* pOwner, CubeGrid* pGrid);
        ~QbertComponent() override = default;

        void Update()      override {}
        void FixedUpdate() override {}
        void Render() const override {}

        void RequestJump(JumpDirection dir);

        int GetRow() const;
        int GetCol() const;

        std::function<void(JumpDirection)> OnJumpStarted{};
        std::function<void()> OnLanded{};

        QbertComponent(const QbertComponent&) = delete;
        QbertComponent& operator=(const QbertComponent&) = delete;
        QbertComponent(QbertComponent&&) = delete;
        QbertComponent& operator=(QbertComponent&&) = delete;

    private:
        void Land();

        CubeGrid* m_pGrid{ nullptr };
        GridMover* m_pMover{ nullptr };

        static constexpr int DeltaRow[4] = { 1,  1, -1, -1 };
        static constexpr int DeltaCol[4] = { 0,  1, -1,  0 };
    };
}
