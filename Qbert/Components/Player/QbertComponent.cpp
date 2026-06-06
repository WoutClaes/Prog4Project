#include "QbertComponent.h"
#include "Components/Grid/GridMover.h"
#include "Components/ScoreComponent.h"
#include "GameObject.h"
#include "GameManager.h"
#include "Sound/ServiceLocator.h"
#include <SDL3/SDL.h>

namespace qbert
{
    QbertComponent::QbertComponent(dae::GameObject* pOwner, CubeGrid* pGrid)
        : GameComponent(pOwner)
        , m_pGrid(pGrid)
    {
        m_pMover = pOwner->GetGameComponent<GridMover>();

        if (m_pMover)
        {
            m_pMover->OnLanded = [this]()
                {
                    Land();
                    if (OnLanded) 
                        OnLanded();
                };
        }
    }

    void QbertComponent::RequestJump(JumpDirection dir)
    {
        if (!m_pMover || m_pMover->IsJumping() || !this->GetOwner()->IsActive()) return;

        const int d = static_cast<int>(dir);
        const int newRow = m_pMover->GetRow() + DeltaRow[d];
        const int newCol = m_pMover->GetCol() + DeltaCol[d];

        if (!CubeGrid::IsValid(newRow, newCol))
        {
            GameManager::GetInstance().OnPlayerDied();
            return;
        }

        if (m_pMover->RequestJump(newRow, newCol))
            if (OnJumpStarted) OnJumpStarted(dir);
    }

    int QbertComponent::GetRow() const { return m_pMover ? m_pMover->GetRow() : 0; }
    int QbertComponent::GetCol() const { return m_pMover ? m_pMover->GetCol() : 0; }

    void QbertComponent::Land()
    {
        if (auto* cube = m_pGrid->GetCube(GetRow(), GetCol()))
        {
            const bool wasTarget = cube->IsTarget();
            cube->Step();

            if (!wasTarget)
                qbert::GameManager::GetInstance().AddScore(25);
        }

        dae::ServiceLocator::GetSoundSystem().Play(dae::ServiceLocator::GetSoundSystem().RegisterSound("Data/Sounds/QBert Jump.wav"), 1.f);

        if (m_pGrid->IsComplete())
        {
            SDL_Log("QbertComponent: Level complete detected!");
            qbert::GameManager::GetInstance().OnLevelComplete();
        }
    }
}
