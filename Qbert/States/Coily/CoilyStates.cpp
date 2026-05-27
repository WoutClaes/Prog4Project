#include "CoilyStates.h"
#include "Components/Enemies/Coily/CoilyComponent.h"
#include "Components/Player/QbertComponent.h"
#include "GameObject.h"

#include <cstdlib>

namespace qbert
{
    // ----------------------------------------------------------------
    // EggState
    // ----------------------------------------------------------------
    std::unique_ptr<ICoilyState> EggState::Update(CoilyComponent& coily, float deltaTime)
    {
        m_JumpTimer += deltaTime;
        if (m_JumpTimer < JumpInterval)
            return nullptr;

        m_JumpTimer = 0.f;

        const bool goRight = (rand() % 2) == 0;
        const JumpDirection dir = goRight
            ? JumpDirection::DownRight
            : JumpDirection::DownLeft;

        coily.Jump(dir);

        if (coily.GetRow() >= CubeGrid::Rows - 1)
            return std::make_unique<SnakeState>(coily.GetQbertObject());

        return nullptr;
    }

    // ----------------------------------------------------------------
    // SnakeState
    // ----------------------------------------------------------------
    std::unique_ptr<ICoilyState> SnakeState::Update(CoilyComponent& coily, float deltaTime)
    {
        m_JumpTimer += deltaTime;
        if (m_JumpTimer < JumpInterval)
            return nullptr;

        m_JumpTimer = 0.f;

        auto* qbertComp = m_pQbertObject
            ? m_pQbertObject->GetGameComponent<QbertComponent>()
            : nullptr;

        if (!qbertComp) return nullptr;

        const int qRow = qbertComp->GetRow();
        const int qCol = qbertComp->GetCol();
        const int myRow = coily.GetRow();
        const int myCol = coily.GetCol();

        const int dRow = qRow - myRow;
        const int dCol = qCol - myCol;

        JumpDirection dir;

        if (dRow >= 0)
        {
            dir = (dCol >= 0)
                ? JumpDirection::DownRight
                : JumpDirection::DownLeft;
        }
        else
        {
            dir = (dCol >= 0)
                ? JumpDirection::UpRight
                : JumpDirection::UpLeft;
        }

        m_LastDir = dir;
        coily.Jump(dir);

        return nullptr;
    }

    void SnakeState::SetStretched()
    {
        switch (m_LastDir)
        {
        case JumpDirection::DownLeft:  
            m_Frame = CoilyFrame::SnakeDownLeftStretched;  
            break;
        case JumpDirection::DownRight: 
            m_Frame = CoilyFrame::SnakeDownRightStretched; 
            break;
        case JumpDirection::UpLeft:    
            m_Frame = CoilyFrame::SnakeUpLeftStretched;    
            break;
        case JumpDirection::UpRight:   
            m_Frame = CoilyFrame::SnakeUpRightStretched;   
            break;
        }
    }

    void SnakeState::SetSquished()
    {
        switch (m_LastDir)
        {
        case JumpDirection::DownLeft:  
            m_Frame = CoilyFrame::SnakeDownLeftSquished;  
            break;
        case JumpDirection::DownRight: 
            m_Frame = CoilyFrame::SnakeDownRightSquished; 
            break;
        case JumpDirection::UpLeft:    
            m_Frame = CoilyFrame::SnakeUpLeftSquished;    
            break;
        case JumpDirection::UpRight:   
            m_Frame = CoilyFrame::SnakeUpRightSquished;   
            break;
        }
    }
}
