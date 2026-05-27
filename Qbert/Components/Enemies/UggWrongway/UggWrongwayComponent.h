#pragma once
#include "Components/GameComponent.h"
#include "Grid/CubeGrid.h"

namespace qbert
{
    class GridMover;

    enum class UggWrongwayType { Ugg, Wrongway };

    enum class UggWrongwayFrame : int
    {
        UggFrame0     = 0,
        UggFrame1     = 1,
        UggFrame2     = 2,
        UggFrame3     = 3,
        WrongwayFrame0 = 4,
        WrongwayFrame1 = 5,
        WrongwayFrame2 = 6,
        WrongwayFrame3 = 7,
    };

    class UggWrongwayComponent final : public dae::GameComponent
    {
    public:
        explicit UggWrongwayComponent(dae::GameObject* pOwner, CubeGrid* pGrid, UggWrongwayType type, int startRow, int startCol);
        ~UggWrongwayComponent() override = default;

        void Update() override;
        void FixedUpdate() override {}
        void Render() const override {}

        int GetRow() const;
        int GetCol() const;

        UggWrongwayFrame GetCurrentFrame() const { return m_Frame; }
        UggWrongwayType  GetType() const { return m_Type; }

        bool IsDead() const { return m_Dead; }

        UggWrongwayComponent(const UggWrongwayComponent&) = delete;
        UggWrongwayComponent& operator=(const UggWrongwayComponent&) = delete;
        UggWrongwayComponent(UggWrongwayComponent&&) = delete;
        UggWrongwayComponent& operator=(UggWrongwayComponent&&) = delete;

    private:
        CubeGrid*         m_pGrid{ nullptr };
        GridMover*        m_pMover{ nullptr };
        UggWrongwayType   m_Type;
        UggWrongwayFrame  m_Frame{ UggWrongwayFrame::UggFrame0 };

        bool  m_Dead{ false };
        float m_JumpTimer{ 0.f };
        static constexpr float JumpInterval{ 1.0f };
    };
}
