#pragma once
#include "Commands/Command.h"
#include "Components/Enemies/Coily/CoilyComponent.h"
#include "States/Coily/CoilyStates.h"

namespace qbert
{
    class CoilyJumpCommand final : public dae::Command
    {
    public:
        CoilyJumpCommand(dae::GameObject* pCoilyObject, JumpDirection dir)
            : m_pCoilyObj(pCoilyObject), m_Direction(dir) {}

        void Execute() override
        {
            if (!m_pCoilyObj) return;

            auto* coilyComp = m_pCoilyObj->GetGameComponent<CoilyComponent>();
            if (!coilyComp || coilyComp->IsDead()) return;

            if (dynamic_cast<SnakeState*>(coilyComp->GetState()) != nullptr)
            {
                coilyComp->Jump(m_Direction);
            }
        }

    private:
        dae::GameObject* m_pCoilyObj;
        JumpDirection m_Direction;
    };
}