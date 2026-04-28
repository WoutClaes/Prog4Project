#include "QbertComponent.h"
#include "Components/TransformComponent.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Sound/ServiceLocator.h"

namespace qbert
{
    // Jump delta per direction:  (deltaRow, deltaCol)
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
            // else: jumped off the pyramid — death logic goes here later
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
            cube->Step();

        dae::ServiceLocator::GetSoundSystem().Play(m_JumpSoundId, 1.f);
    }

    void QbertComponent::UpdateScreenPosition()
    {
        if (auto* cube = m_pGrid->GetCube(m_Row, m_Col))
        {
            if (auto* transform = GetOwner()->GetGameComponent<dae::TransformComponent>())
            {
                // Offset upward so Qbert sits on top of the cube, not inside it
                // The cube tile is 64px tall (32 * scale=2), Qbert sprite is ~16px tall scaled
                // so offset by roughly 1/3 tile height upward
                constexpr float offsetY = -20.f;
                transform->SetLocalPosition(cube->GetScreenX(), cube->GetScreenY() + offsetY);
            }
        }
    }
}
