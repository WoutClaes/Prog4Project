#include "Cube.h"

namespace qbert
{
    Cube::Cube(int row, int col)
        : m_Row(row)
        , m_Col(col)
    {}

    void Cube::Step()
    {
        if (m_State == CubeColorState::Target)
        {
            if (m_Reverts)
            {
                m_State = (m_StepsToTarget == 2) ? CubeColorState::Intermediate : CubeColorState::Base;
            }
            return;
        }

        if (m_StepsToTarget == 1)
        {
            m_State = CubeColorState::Target;
        }
        else
        {
            if (m_State == CubeColorState::Base)
                m_State = CubeColorState::Intermediate;
            else if (m_State == CubeColorState::Intermediate)
                m_State = CubeColorState::Target;
        }
    }

    void Cube::Revert()
    {
        if (m_State == CubeColorState::Target)
            m_State = (m_StepsToTarget == 2) ? CubeColorState::Intermediate : CubeColorState::Base;
        else if (m_State == CubeColorState::Intermediate)
            m_State = CubeColorState::Base;
    }

    void Cube::Reset()
    {
        m_State = CubeColorState::Base;
    }
}
