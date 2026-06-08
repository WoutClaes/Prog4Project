#include "CoilyComponent.h"
#include "Components/Grid/GridMover.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Grid/CubeGrid.h"
#include "GameManager.h"

namespace qbert
{
    CoilyComponent::CoilyComponent(dae::GameObject* pOwner, dae::GameObject* pQbertObject)
        : GameComponent(pOwner)
        , m_pQbertObject(pQbertObject)
        , m_pState(std::make_unique<EggState>())
    {
        m_pMover = pOwner->GetGameComponent<GridMover>();

        m_pMover->OnLanded = [this]()
        {
            if (!CubeGrid::IsValid(GetRow(), GetCol()))
            {
                qbert::GameManager::GetInstance().AddScore(500);
                Die();
                return;
            }
            m_LandPauseTimer = LandPause;
        };
    }

    void CoilyComponent::Update()
    {
        if (m_IsDead) return;

        if (m_pMover && m_pMover->IsJumping()) return;

        if (m_LandPauseTimer > 0.8f)
        {
            m_LandPauseTimer -= dae::GameTime::GetInstance().GetDeltaTime();
            return;
        }

        const float dt = dae::GameTime::GetInstance().GetDeltaTime();
        bool skipAIUpdate = m_IsPlayerControlled && dynamic_cast<SnakeState*>(m_pState.get()) != nullptr;

        if (!skipAIUpdate)
        {
            auto nextState = m_pState->Update(*this, dt);
            if (nextState)
                m_pState = std::move(nextState);
        }
    }

    CoilyFrame CoilyComponent::GetCurrentFrame() const
    {
        const CoilyFrame baseFrame = m_pState->GetFrame();

        if (m_pMover && m_pMover->IsJumping())
        {
            const float progress = m_pMover->GetJumpProgress();

            if (dynamic_cast<EggState*>(m_pState.get()))
            {
                return progress < 0.8f ? CoilyFrame::EggNormal : CoilyFrame::EggSquished;
            }
            if (auto* snake = dynamic_cast<SnakeState*>(m_pState.get()))
            {
                if (progress < 0.8f) 
                { 
                    snake->SetStretched(); 
                }
                else                 
                { 
                    snake->SetSquished();
                }
                return m_pState->GetFrame();
            }
        }

        if (m_LandPauseTimer > 0.f)
        {
            if (dynamic_cast<EggState*>(m_pState.get()))
                return CoilyFrame::EggSquished;
            if (auto* snake = dynamic_cast<SnakeState*>(m_pState.get()))
            {
                snake->SetSquished();
                return m_pState->GetFrame();
            }
        }

        return baseFrame;
    }

    void CoilyComponent::Jump(JumpDirection dir)
    {
        if (!m_pMover) return;

        const int d      = static_cast<int>(dir);
        const int newRow = GetRow() + DeltaRow[d];
        const int newCol = GetCol() + DeltaCol[d];

        if (!CubeGrid::IsValid(newRow, newCol))
        {
            m_pMover->JumpOff(DeltaRow[d], DeltaCol[d]);
            return;
        }

        if (!m_pMover->RequestJump(newRow, newCol))
        {
            Die();
        }
    }

    void CoilyComponent::Die()
    {
        if (m_IsDead) return;
        m_IsDead = true;
        if (OnDeathCallback) OnDeathCallback();
    }

    int CoilyComponent::GetRow() const { return m_pMover ? m_pMover->GetRow() : 0; }
    int CoilyComponent::GetCol() const { return m_pMover ? m_pMover->GetCol() : 0; }
}
