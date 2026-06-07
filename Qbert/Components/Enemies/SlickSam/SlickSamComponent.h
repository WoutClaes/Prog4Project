#pragma once
#include "Components/GameComponent.h"
#include "Grid/CubeGrid.h"
#include <functional>

namespace qbert
{
    class GridMover;

    enum class SlickSamType { Slick, Sam };

    enum class SlickSamFrame : int
    {
        SlickLeft  = 0,
        SlickRight = 1,
        SamLeft    = 2,
        SamRight   = 3,
    };

    class SlickSamComponent final : public dae::GameComponent
    {
    public:
        explicit SlickSamComponent(dae::GameObject* pOwner, CubeGrid* pGrid, SlickSamType type);
        ~SlickSamComponent() override = default;

        void Update()      override;
        void FixedUpdate() override {}
        void Render() const override {}

        int GetRow() const;
        int GetCol() const;

        SlickSamFrame GetCurrentFrame() const { return m_Frame; }
        SlickSamType  GetType()         const { return m_Type; }

        bool IsDead() const { return m_Dead; }
        void Die();
        void Catch();

        std::function<void()> OnDeathCallback;

        SlickSamComponent(const SlickSamComponent&) = delete;
        SlickSamComponent& operator=(const SlickSamComponent&) = delete;
        SlickSamComponent(SlickSamComponent&&) = delete;
        SlickSamComponent& operator=(SlickSamComponent&&) = delete;

    private:
        void OnLanded();

        CubeGrid*     m_pGrid{ nullptr };
        GridMover*    m_pMover{ nullptr };
        SlickSamType  m_Type;
        SlickSamFrame m_Frame{ SlickSamFrame::SlickLeft };

        bool  m_Dead{ false };
        float m_JumpTimer{ 0.f };
        static constexpr float JumpInterval{ 0.9f };
        float m_FallTimer{ -1.f };
        static constexpr float FallDuration{ 0.3f };
    };
}
