#include "UggWrongwayComponent.h"
#include "Components/Grid/GridMover.h"
#include "GameObject.h"
#include "GameTime.h"
#include <cstdlib>

namespace qbert
{

    UggWrongwayComponent::UggWrongwayComponent(dae::GameObject* pOwner, UggWrongwayType type)
        : GameComponent(pOwner)
        , m_Type(type)
    {
        m_pMover = pOwner->GetGameComponent<GridMover>();

        m_Frame = (m_Type == UggWrongwayType::Ugg)
            ? UggWrongwayFrame::UggFrame0
            : UggWrongwayFrame::WrongwayFrame0;
    }

    void UggWrongwayComponent::Update()
    {
        if (m_Dead) return;
        if (m_pMover && m_pMover->IsJumping()) return;

        m_JumpTimer += dae::GameTime::GetInstance().GetDeltaTime();
        if (m_JumpTimer < JumpInterval) return;
        m_JumpTimer = 0.f;

        static constexpr int DeltaRow[4] = { 1,  1, -1, -1 };
        static constexpr int DeltaCol[4] = { 0,  1, -1,  0 };

        const bool goRight = (rand() % 2) == 0;

        JumpDirection preferred = goRight ? JumpDirection::UpRight : JumpDirection::UpLeft;
        JumpDirection fallback = goRight ? JumpDirection::UpLeft : JumpDirection::UpRight;

        int d = static_cast<int>(preferred);
        int newRow = GetRow() + DeltaRow[d];
        int newCol = GetCol() + DeltaCol[d];

        if (!CubeGrid::IsValid(newRow, newCol))
        {
            d = static_cast<int>(fallback);
            newRow = GetRow() + DeltaRow[d];
            newCol = GetCol() + DeltaCol[d];
        }

        const bool finalGoRight = (d == static_cast<int>(JumpDirection::UpRight));
        if (m_Type == UggWrongwayType::Ugg)
            m_Frame = finalGoRight ? UggWrongwayFrame::UggFrame1 : UggWrongwayFrame::UggFrame0;
        else
            m_Frame = finalGoRight ? UggWrongwayFrame::WrongwayFrame1 : UggWrongwayFrame::WrongwayFrame0;

        if (!m_pMover->RequestJump(newRow, newCol))
            m_Dead = true;
    }

    int UggWrongwayComponent::GetRow() const { return m_pMover ? m_pMover->GetRow() : 0; }
    int UggWrongwayComponent::GetCol() const { return m_pMover ? m_pMover->GetCol() : 0; }
}
