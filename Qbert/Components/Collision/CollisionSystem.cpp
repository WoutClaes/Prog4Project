#include "CollisionSystem.h"

#include "Components/Player/QbertComponent.h"
#include "Components/Enemies/Coily/CoilyComponent.h"
#include "Components/Enemies/SlickSam/SlickSamComponent.h"
#include "Components/Enemies/UggWrongway/UggWrongwayComponent.h"
#include "GameManager.h"

namespace qbert
{
    void CollisionSystem::Update()
    {
        m_StartupTimer += dae::GameTime::GetInstance().GetDeltaTime();
        if (m_StartupTimer < 1.f) return;

        CheckQbertVsCoily();
        CheckQbertVsSlickSam();
        CheckQbertVsUggWrongway();
    }

    void CollisionSystem::CheckQbertVsCoily()
    {
        for (auto* qbert : m_Qberts)
        {
            if (!qbert || qbert->IsRidingDisk()) continue;

            for (auto* coily : m_Coilys)
            {
                if (!coily || coily->IsDead()) continue;

                if (qbert->GetRow() == coily->GetRow() &&
                    qbert->GetCol() == coily->GetCol())
                {
                    qbert->Die();
                    return;
                }
            }
        }
    }

    void CollisionSystem::CheckQbertVsSlickSam()
    {
        for (auto* qbert : m_Qberts)
        {
            if (!qbert || qbert->IsRidingDisk()) continue;

            for (auto* ss : m_SlickSams)
            {
                if (!ss || ss->IsDead()) continue;

                if (qbert->GetRow() == ss->GetRow() &&
                    qbert->GetCol() == ss->GetCol())
                {
                    ss->Catch();
                    GameManager::GetInstance().AddScore(300);
                }
            }
        }
    }

    void CollisionSystem::CheckQbertVsUggWrongway()
    {
        for (auto* qbert : m_Qberts)
        {
            if (!qbert || qbert->IsRidingDisk()) continue;

            for (auto* uw : m_UggWrongways)
            {
                if (!uw || uw->IsDead()) continue;

                if (qbert->GetRow() == uw->GetRow() &&
                    qbert->GetCol() == uw->GetCol())
                {
                    qbert->Die();
                    return;
                }
            }
        }
    }
}
