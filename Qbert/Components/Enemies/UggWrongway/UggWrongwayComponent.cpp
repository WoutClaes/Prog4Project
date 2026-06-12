#include "UggWrongwayComponent.h"
#include "Components/Grid/GridMover.h"
#include "GameObject.h"
#include "GameTime.h"
#include <cstdlib>
#include "Sound/ServiceLocator.h"

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

        const bool choice = (rand() % 2) == 0;
        int deltaRow = 0;
        int deltaCol = 0;

        if (m_Type == UggWrongwayType::Wrongway)
        {
            if (choice) { deltaRow = -1; deltaCol = 0; }
            else { deltaRow = 0;  deltaCol = 1; }

            m_Frame = choice ? UggWrongwayFrame::WrongwayFrame1 : UggWrongwayFrame::WrongwayFrame0;
        }
        else
        {
            if (choice) { deltaRow = -1; deltaCol = -1; }
            else { deltaRow = 0;  deltaCol = -1; }

            m_Frame = choice ? UggWrongwayFrame::UggFrame1 : UggWrongwayFrame::UggFrame0;
        }

        int newRow = GetRow() + deltaRow;
        int newCol = GetCol() + deltaCol;

        if (!CubeGrid::IsValid(newRow, newCol))
        {
            m_pMover->JumpOff(deltaRow, deltaCol);
            Die();
            return;
        }

        dae::ServiceLocator::GetSoundSystem().Play(dae::ServiceLocator::GetSoundSystem().RegisterSound("Data/Sounds/Other Foes Jump.wav"), 1.f);
        m_pMover->RequestJump(newRow, newCol);
    }

    int UggWrongwayComponent::GetRow() const 
    { 
        return m_pMover ? m_pMover->GetRow() : 0; 
    }
    int UggWrongwayComponent::GetCol() const 
    { 
        return m_pMover ? m_pMover->GetCol() : 0; 
    }

    void UggWrongwayComponent::Die()
    {
        if(m_Dead) return;
        m_Dead = true;
        if (OnDeathCallback) OnDeathCallback();
    }
}
