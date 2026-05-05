#pragma once
#include "Grid/CubeGrid.h"
#include "IState.h"

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

    // Coily state interface
    using ICoilyState = IState<CoilyComponent, CoilyFrame>;

    // EggState
    class EggState final : public ICoilyState
    {
    public:
        ICoilyState* Update(CoilyComponent& coily, float deltaTime) override;
        CoilyFrame GetFrame() const { return m_Frame; }

    private:
        float      m_JumpTimer{ 0.f };
        static constexpr float JumpInterval{ 1.2f };
        CoilyFrame m_Frame{ CoilyFrame::EggNormal };
    };

    // SnakeState
    class SnakeState final : public ICoilyState
    {
    public:
        explicit SnakeState(dae::GameObject* pQbertObject)
            : m_pQbertObject(pQbertObject) {}

        ICoilyState* Update(CoilyComponent& coily, float deltaTime) override;
        CoilyFrame GetFrame() const { return m_Frame; }

    private:
        dae::GameObject* m_pQbertObject{ nullptr };

        float      m_JumpTimer{ 0.f };
        static constexpr float JumpInterval{ 1.f };
        CoilyFrame m_Frame{ CoilyFrame::SnakeUpRightStretched };
    };
}
