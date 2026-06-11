#pragma once
#include "Grid/CubeGrid.h"
#include "IState.h"
#include <memory>

namespace dae { class GameObject; }

namespace qbert
{
    class CoilyComponent;

    enum class CoilyFrame : int
    {
        EggSquished = 0,
        EggNormal = 1,
        SnakeUpRightSquished = 2,
        SnakeUpRightStretched = 3,
        SnakeUpLeftSquished = 4,
        SnakeUpLeftStretched = 5,
        SnakeDownRightSquished = 6,
        SnakeDownRightStretched = 7,
        SnakeDownLeftSquished = 8,
        SnakeDownLeftStretched = 9,
    };

    using ICoilyState = IState<CoilyComponent, CoilyFrame>;

    // ----------------------------------------------------------------
    // EggState
    // ----------------------------------------------------------------
    class EggState final : public ICoilyState
    {
    public:
        std::unique_ptr<ICoilyState> Update(CoilyComponent& coily, float deltaTime) override;
        CoilyFrame GetFrame() const override { return m_Frame; }

        void SetStretched() { m_Frame = CoilyFrame::EggNormal;   }
        void SetSquished() { m_Frame = CoilyFrame::EggSquished; }

    private:
        float m_JumpTimer{ 0.f };
        static constexpr float JumpInterval{ 1.2f };
        CoilyFrame m_Frame{ CoilyFrame::EggNormal };
    };

    // ----------------------------------------------------------------
    // SnakeState
    // ----------------------------------------------------------------
    class SnakeState final : public ICoilyState
    {
    public:
        explicit SnakeState(dae::GameObject* pQbertObject)
            : m_pQbertObject(pQbertObject) {}

        std::unique_ptr<ICoilyState> Update(CoilyComponent& coily, float deltaTime) override;
        CoilyFrame GetFrame() const override { return m_Frame; }

        void SetStretched();
        void SetSquished();

    private:
        dae::GameObject* m_pQbertObject{ nullptr };

        float m_JumpTimer{ 0.f };
        static constexpr float JumpInterval{ 1.f };
        CoilyFrame m_Frame{ CoilyFrame::SnakeDownRightSquished };
        JumpDirection m_LastDir{ JumpDirection::DownLeft };
    };
}
