#include "SlickSamComponent.h"
#include "Components/Grid/GridMover.h"
#include "Components/ScoreComponent.h"
#include "GameObject.h"
#include "GameTime.h"
#include <cstdlib>

#include <iostream>

namespace qbert
{
    SlickSamComponent::SlickSamComponent(dae::GameObject* pOwner, CubeGrid* pGrid, SlickSamType type)
        : GameComponent(pOwner)
        , m_pGrid(pGrid)
        , m_Type(type)
    {
        m_pMover = pOwner->GetGameComponent<GridMover>();

        m_Frame = (m_Type == SlickSamType::Slick)
            ? SlickSamFrame::SlickLeft
            : SlickSamFrame::SamLeft;

        if (m_pMover)
        {
            m_pMover->OnLanded = [this]() { OnLanded(); };
        }
    }

    void SlickSamComponent::Update()
    {
        if (m_Dead) return;
        if (m_pMover && m_pMover->IsJumping()) return;

        m_JumpTimer += dae::GameTime::GetInstance().GetDeltaTime();
        if (m_JumpTimer < JumpInterval) return;
        m_JumpTimer = 0.f;

        const bool goRight = (rand() % 2) == 0;
        const int dir      = goRight
            ? static_cast<int>(JumpDirection::DownRight)
            : static_cast<int>(JumpDirection::DownLeft);

        if (m_Type == SlickSamType::Slick)
            m_Frame = goRight ? SlickSamFrame::SlickRight : SlickSamFrame::SlickLeft;
        else
            m_Frame = goRight ? SlickSamFrame::SamRight   : SlickSamFrame::SamLeft;

        static constexpr int DeltaRow[2] = { 1, 1 };
        static constexpr int DeltaCol[2] = { 0, 1 };

        const int newRow = GetRow() + DeltaRow[dir];
        const int newCol = GetCol() + DeltaCol[dir];

        m_pMover->RequestJump(newRow, newCol);
    }

    void SlickSamComponent::OnLanded()
    {
        if (m_Dead) return;

        if (auto* cube = m_pGrid->GetCube(GetRow(), GetCol()))
            cube->Revert();

        std::cout << m_pGrid->GetCube(GetRow(), GetCol())->GetCol() << " | " << m_pGrid->GetCube(GetRow(), GetCol())->GetRow() << "\n";

        if (GetRow() >= CubeGrid::Rows - 1)
            m_Dead = true;
    }

    void SlickSamComponent::Catch()
    {
        if (m_Dead) return;
        m_Dead = true;
    }

    int SlickSamComponent::GetRow() const { return m_pMover ? m_pMover->GetRow() : 0; }
    int SlickSamComponent::GetCol() const { return m_pMover ? m_pMover->GetCol() : 0; }
}
