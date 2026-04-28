#pragma once
#include <cstdint>

namespace qbert
{
    enum class CubeColorState
    {
        Base,
        Intermediate,
        Target
    };

    class Cube final
    {
    public:
        Cube() : m_Row(0), m_Col(0) {}
        Cube(int row, int col);

        void Step();
        void Revert();
        void Reset();

        CubeColorState GetState()  const { return m_State; }
        bool           IsTarget()  const { return m_State == CubeColorState::Target; }

        int GetRow() const { return m_Row; }
        int GetCol() const { return m_Col; }

        void  SetScreenPos(float x, float y) { m_ScreenX = x; m_ScreenY = y; }
        float GetScreenX() const { return m_ScreenX; }
        float GetScreenY() const { return m_ScreenY; }

        void SetStepsToTarget(int steps) { m_StepsToTarget = steps; }
        int  GetStepsToTarget()    const { return m_StepsToTarget; }

    private:
        int m_Row;
        int m_Col;

        CubeColorState m_State{ CubeColorState::Base };
        int   m_StepsToTarget{ 1 };
        float m_ScreenX{ 0.f };
        float m_ScreenY{ 0.f };
    };
}
