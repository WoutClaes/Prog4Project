#include "CoilyComponent.h"
#include "Components/Grid/GridMover.h"
#include "GameObject.h"
#include "GameTime.h"

namespace qbert
{
    CoilyComponent::CoilyComponent(dae::GameObject* pOwner, CubeGrid* pGrid, dae::GameObject* pQbertObject)
        : GameComponent(pOwner)
        , m_pGrid(pGrid)
        , m_pQbertObject(pQbertObject)
        , m_pState(std::make_unique<EggState>())
    {
        m_pMover = pOwner->GetGameComponent<GridMover>();

        m_pMover->OnMidAir = [this]()
            {
                // Switch to stretched frame mid-air
                m_pState->SetStretched();
            };

        m_pMover->OnLanded = [this]()
            {
                // Switch to squished frame on landing
                m_pState->SetSquished();
            };
    }

    void CoilyComponent::Update()
    {
        if (m_pMover && m_pMover->IsJumping()) return;

        const float dt = dae::GameTime::GetInstance().GetDeltaTime();
        ICoilyState* nextState = m_pState->Update(*this, dt);
        if (nextState)
            m_pState.reset(nextState);
    }

    void CoilyComponent::Jump(JumpDirection dir)
    {
        if (!m_pMover) return;

        const int d = static_cast<int>(dir);
        const int newRow = GetRow() + DeltaRow[d];
        const int newCol = GetCol() + DeltaCol[d];

        m_pMover->RequestJump(newRow, newCol);
    }

    int CoilyComponent::GetRow() const { return m_pMover ? m_pMover->GetRow() : 0; }
    int CoilyComponent::GetCol() const { return m_pMover ? m_pMover->GetCol() : 0; }
}
