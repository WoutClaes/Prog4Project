#include "DiskComponent.h"
#include "GridMover.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Components/TransformComponent.h"
#include <Components/GameComponent.h>

namespace qbert
{
    DiskComponent::DiskComponent(dae::GameObject* pOwner, dae::GameObject* pQbertObj, GridMover* pQbertMover, int diskRow, int diskCol, const glm::vec3& topPos)
        : GameComponent(pOwner)
        , m_pQbertObj(pQbertObj)
        , m_pQbertMover(pQbertMover)
        , m_Row(diskRow)
        , m_Col(diskCol)
        , m_TopPos(topPos)
    {}

    void DiskComponent::Update()
    {
        if (!m_IsActive) return;

        auto* pTransform = GetOwner()->GetGameComponent<dae::TransformComponent>();

        if (!m_IsMoving)
        {
            if (m_pQbertMover->GetRow() == m_Row && m_pQbertMover->GetCol() == m_Col)
            {
                if (!m_pQbertMover->IsJumping())
                {
                    m_IsMoving = true;
                    m_StartPos = pTransform->GetLocalTransform().GetPosition();

                    m_pQbertObj->SetParent(GetOwner(), true);
                }
            }
            return;
        }

        m_MoveTimer += dae::GameTime::GetInstance().GetDeltaTime();
        float t = m_MoveTimer / 2.0f;

        if (t >= 1.f)
        {
            t = 1.f;
            m_IsActive = false;

            m_pQbertObj->SetParent(nullptr, true);

            m_pQbertMover->SetGridPosition(0, 0, m_TopPos);

            GetOwner()->SetActive(false);
        }

        glm::vec3 currentPos = glm::mix(m_StartPos, m_TopPos, t);
        pTransform->SetLocalPosition(currentPos);
    }
}