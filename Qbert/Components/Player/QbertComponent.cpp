#include "QbertComponent.h"
#include "Components/TransformComponent.h"
#include "Components/ScoreComponent.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Sound/ServiceLocator.h"

namespace qbert
{
    static constexpr int DeltaRow[4] = {  1,  1, -1, -1 };
    static constexpr int DeltaCol[4] = {  0,  1, -1,  0 };

    QbertComponent::QbertComponent(dae::GameObject* pOwner, CubeGrid* pGrid, int row, int col)
        : GameComponent(pOwner)
        , m_pGrid(pGrid)
        , m_Row(row)
        , m_Col(col)
    {
        m_JumpSoundId = dae::ServiceLocator::GetSoundSystem()
            .RegisterSound("Data/Sounds/QBert Jump.wav");
        UpdateScreenPosition();
    }

    void QbertComponent::Update()
    {
        if (m_IsJumping)
        {
            m_JumpTimer -= dae::GameTime::GetInstance().GetDeltaTime();
            if (m_JumpTimer <= 0.f)
            {
                m_IsJumping = false;
                Land();
            }
            return;
        }

        if (m_JumpPending)
        {
            m_JumpPending = false;

            const int dir   = static_cast<int>(m_PendingDir);
            const int newRow = m_Row + DeltaRow[dir];
            const int newCol = m_Col + DeltaCol[dir];

            if (CubeGrid::IsValid(newRow, newCol))
            {
                m_Row = newRow;
                m_Col = newCol;
                m_IsJumping = true;
                m_JumpTimer = JumpDuration;
                UpdateScreenPosition();
            }
            // else: jumped off the pyramid - death logic goes here later
        }
    }

    void QbertComponent::RequestJump(JumpDirection dir)
    {
        if (!m_IsJumping)
        {
            m_PendingDir  = dir;
            m_JumpPending = true;
        }
    }

    bool QbertComponent::CanJump(JumpDirection dir) const
    {
        const int d      = static_cast<int>(dir);
        const int newRow = m_Row + DeltaRow[d];
        const int newCol = m_Col + DeltaCol[d];
        return CubeGrid::IsValid(newRow, newCol);
    }

    void QbertComponent::Land()
    {
        if (auto* cube = m_pGrid->GetCube(m_Row, m_Col))
        {
            const bool wasTarget = cube->IsTarget();
            cube->Step();

            if (!wasTarget)
            {
                if (auto* score = GetOwner()->GetGameComponent<dae::ScoreComponent>())
                    score->AddPoints(25);  // 25pts per cube, 500 to win = all 28 cubes + a few revisits
            }
        }

        dae::ServiceLocator::GetSoundSystem().Play(m_JumpSoundId, 1.f);
    }

    void QbertComponent::UpdateScreenPosition()
    {
        if (auto* cube = m_pGrid->GetCube(m_Row, m_Col))
        {
            if (auto* transform = GetOwner()->GetGameComponent<dae::TransformComponent>())
            {
                constexpr float offsetY = -20.f;
                transform->SetLocalPosition(cube->GetScreenX(), cube->GetScreenY() + offsetY);
            }
        }
    }
}
