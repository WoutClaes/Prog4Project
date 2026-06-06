#pragma once
#include "Command.h"

namespace dae
{
    class ChangeLetterCommand final : public Command
    {
    public:
        ChangeLetterCommand(NameInputComponent* comp, int dir) : m_pComp(comp), m_Direction(dir) {}
        void Execute() override
        {
            if (m_pComp) m_pComp->ChangeLetter(m_Direction);
        }
    private:
        NameInputComponent* m_pComp;
        int m_Direction;
    };

    class NextSlotCommand final : public Command
    {
    public:
        NextSlotCommand(NameInputComponent* comp) : m_pComp(comp) {}
        void Execute() override
        {
            if (m_pComp) m_pComp->NextSlot();
        }
    private:
        NameInputComponent* m_pComp;
    };
}