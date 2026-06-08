#include "QbertComponent.h"
#include "Components/Grid/GridMover.h"
#include "GameObject.h"
#include "GameManager.h"
#include "Sound/ServiceLocator.h"
#include <Components/GameComponent.h>
#include <glm/glm.hpp>
#include <SDL3/SDL_log.h>
#include <Grid/CubeGrid.h>

namespace qbert
{
    QbertComponent::QbertComponent(dae::GameObject* pOwner, CubeGrid* pGrid, int playerIdx)
        : GameComponent(pOwner)
        , m_pGrid(pGrid)
        , m_PlayerIdx(playerIdx)
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

    void qbert::QbertComponent::RequestJump(JumpDirection dir)
    {
        if (!m_pMover || m_pMover->IsJumping() || !this->GetOwner()->IsActive()) return;

        const int d = static_cast<int>(dir);
        const int newRow = m_pMover->GetRow() + DeltaRow[d];
        const int newCol = m_pMover->GetCol() + DeltaCol[d];

        for (auto it = m_Disks.begin(); it != m_Disks.end(); ++it)
        {
            if (it->row == newRow && it->col == newCol)
            {
                if (m_pMover->ForceJump(newRow, newCol, it->screenPos))
                {
                    if (OnJumpStarted) OnJumpStarted(dir);
                }
                return;
            }
        }

        if (!CubeGrid::IsValid(newRow, newCol))
        {
            if (m_pMover->JumpOff(DeltaRow[d], DeltaCol[d]))
            {
                if (OnJumpStarted) OnJumpStarted(dir);
            }
            return;
        }

        if (m_pMover->RequestJump(newRow, newCol))
        {
            if (OnJumpStarted) OnJumpStarted(dir);
        }
    }

    void QbertComponent::Die()
    {
        GameManager::GetInstance().OnPlayerDied(GetOwner());
    }

    void QbertComponent::AddDisk(int row, int col, const glm::vec3& screenPos)
    {
        m_Disks.push_back({ row, col, screenPos });
    }

    int QbertComponent::GetRow() const { return m_pMover ? m_pMover->GetRow() : 0; }
    int QbertComponent::GetCol() const { return m_pMover ? m_pMover->GetCol() : 0; }

    void QbertComponent::Land()
    {
        for (auto it = m_Disks.begin(); it != m_Disks.end(); ++it)
        {
            if (it->row == GetRow() && it->col == GetCol())
            {
                m_Disks.erase(it);
                return;
            }
        }

        if (!CubeGrid::IsValid(GetRow(), GetCol()))
        {
            Die();
            return;
        }

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
