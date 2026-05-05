#pragma once
#include "Commands/Command.h"
#include "Components/Player/QbertComponent.h"
#include "GameObject.h"

namespace qbert
{
    class JumpCommand final : public dae::Command
    {
    public:
        JumpCommand(dae::GameObject* pGO, JumpDirection dir)
            : m_pGO(pGO)
            , m_Dir(dir)
        {}

        void Execute() override
        {
            if (auto* qbert = m_pGO->GetGameComponent<QbertComponent>())
                qbert->RequestJump(m_Dir);
        }

    private:
        dae::GameObject* m_pGO{ nullptr };
        JumpDirection    m_Dir;
    };
}
