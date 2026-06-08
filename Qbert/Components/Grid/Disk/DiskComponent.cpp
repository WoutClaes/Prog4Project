#include "DiskComponent.h"
#include "Components/Grid/GridMover.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Components/TransformComponent.h"
#include "Components/GameComponent.h"
#include "Components/Player/QbertComponent.h"

namespace qbert
{
    DiskComponent::DiskComponent(dae::GameObject* pOwner, const std::vector<dae::GameObject*>& players, int diskRow, int diskCol, const glm::vec3& topPos)
        : GameComponent(pOwner)
        , m_Players(players)
        , m_Row(diskRow)
        , m_Col(diskCol)
        , m_TopPos(topPos)
    {
        if (auto* pTransform = pOwner->GetGameComponent<dae::TransformComponent>())
        {
            m_StartPos = pTransform->GetLocalTransform().GetPosition();
        }
    }

    void DiskComponent::Update()
    {
        if (!m_IsActive) return;

        auto* pTransform = GetOwner()->GetGameComponent<dae::TransformComponent>();
        if (!pTransform) return;

        if (!m_IsMoving)
        {
            for (auto* pPlayer : m_Players)
            {
                if (!pPlayer || !pPlayer->IsActive()) continue;

                auto* pMover = pPlayer->GetGameComponent<GridMover>();
                if (!pMover) continue;

                if (pMover->GetRow() == m_Row && pMover->GetCol() == m_Col)
                {
                    if (!pMover->IsJumping())
                    {
                        m_pRidingObj = pPlayer;
                        m_pRidingMover = pMover;

                        m_pRidingObj->GetGameComponent<QbertComponent>()->SetRidingDisk(true, m_Row, m_Col);
                        m_IsMoving = true;
                        m_StartPos = pTransform->GetLocalTransform().GetPosition();

                        auto* qTransform = m_pRidingObj->GetGameComponent<dae::TransformComponent>();
                        if (qTransform)
                        {
                            m_QbertOffset = qTransform->GetLocalTransform().GetPosition() - m_StartPos;
                        }
                        break;
                    }
                }
            }
            return;
        }

        m_MoveTimer += dae::GameTime::GetInstance().GetDeltaTime();

        const float travelDuration = 2.0f;
        float t = m_MoveTimer / travelDuration;

        if (t >= 1.f)
        {
            if (m_pRidingObj)
                m_pRidingObj->GetGameComponent<QbertComponent>()->SetRidingDisk(false);

            t = 1.f;
            m_IsActive = false;
            GetOwner()->SetActive(false);

            if (m_pRidingMover)
                m_pRidingMover->ForceJump(0, 0, m_TopPos);

            pTransform->SetLocalPosition({ -9999.f, -9999.f, 0.f });
            return;
        }

        glm::vec3 currentPos = m_StartPos + (m_TopPos - m_StartPos) * t;
        pTransform->SetLocalPosition(currentPos);

        if (m_pRidingObj)
        {
            auto* qTransform = m_pRidingObj->GetGameComponent<dae::TransformComponent>();
            if (qTransform)
            {
                qTransform->SetLocalPosition(currentPos + m_QbertOffset);
            }
        }
    }
}