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
        explicit CoilyComponent(dae::GameObject* pOwner,
            CubeGrid* pGrid,
            dae::GameObject* pQbertObject,
            int startRow, int startCol);
        ~CoilyComponent() override = default;

        void Update()      override;
        void FixedUpdate() override {}
        void Render() const override {}

        void Jump(JumpDirection dir);

        int GetRow() const;
        int GetCol() const;

        CoilyFrame       GetCurrentFrame() const { return m_pState->GetFrame(); }
        dae::GameObject* GetQbertObject()  const { return m_pQbertObject; }

        CoilyComponent(const CoilyComponent&) = delete;
        CoilyComponent& operator=(const CoilyComponent&) = delete;
        CoilyComponent(CoilyComponent&&) = delete;
        CoilyComponent& operator=(CoilyComponent&&) = delete;

    private:
        CubeGrid* m_pGrid{ nullptr };
        dae::GameObject* m_pQbertObject{ nullptr };
        GridMover* m_pMover{ nullptr };

        static constexpr int DeltaRow[4] = { 1,  1, -1, -1 };
        static constexpr int DeltaCol[4] = { 0,  1, -1,  0 };

        std::unique_ptr<ICoilyState> m_pState{};
    };
}
