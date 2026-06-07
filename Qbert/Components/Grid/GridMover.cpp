#include "GridMover.h"
#include "Components/TransformComponent.h"
#include "GameObject.h"
#include "GameTime.h"

namespace qbert
{
    GridMover::GridMover(dae::GameObject* pOwner, CubeGrid* pGrid,
        int startRow, int startCol, float offsetX, float offsetY)
        : GameComponent(pOwner)
        , m_pGrid(pGrid)
        , m_Row(startRow)
        , m_Col(startCol)
        , m_OffsetX(offsetX)
        , m_OffsetY(offsetY)
    {
        const glm::vec3 pos = CubeScreenPos(startRow, startCol);
        UpdateTransform(pos);
        m_StartPos = pos;
        m_TargetPos = pos;
    }

    void GridMover::Update()
    {
        if (!m_IsJumping) return;

        m_JumpTimer += dae::GameTime::GetInstance().GetDeltaTime();
        const float t = m_JumpTimer / JumpDuration;

        if (t >= 1.f)
        {
            m_IsJumping = false;
            m_JumpTimer = 0.f;
            UpdateTransform(m_TargetPos);
            if (OnLanded) OnLanded();
            return;
        }

        const float arc = -4.f * t * (t - 1.f);
        const float arcHeight = 24.f;

        glm::vec3 pos = glm::mix(m_StartPos, m_TargetPos, t);
        pos.y -= arc * arcHeight;

        UpdateTransform(pos);
    }

    bool GridMover::RequestJump(int newRow, int newCol)
    {
        if (m_IsJumping) return false;
        if (!CubeGrid::IsValid(newRow, newCol)) return false;

        m_StartPos = CubeScreenPos(m_Row, m_Col);
        m_TargetPos = CubeScreenPos(newRow, newCol);
        m_Row = newRow;
        m_Col = newCol;
        m_IsJumping = true;
        m_MidAirFired = false;
        m_JumpTimer = 0.f;

        return true;
    }

    bool GridMover::ForceJump(int newRow, int newCol, const glm::vec3& targetPos)
    {
        if (m_IsJumping) return false;

        m_StartPos = GetOwner()->GetGameComponent<dae::TransformComponent>()->GetLocalTransform().GetPosition();
        m_TargetPos = targetPos;
        m_Row = newRow;
        m_Col = newCol;
        m_IsJumping = true;
        m_MidAirFired = false;
        m_JumpTimer = 0.f;

        return true;
    }

    void GridMover::SetGridPosition(int row, int col, const glm::vec3& screenPos)
    {
        m_Row = row;
        m_Col = col;
        UpdateTransform(screenPos);
    }

    glm::vec3 GridMover::CubeScreenPos(int row, int col) const
    {
        if (auto* cube = m_pGrid->GetCube(row, col))
            return { cube->GetScreenX() + m_OffsetX, cube->GetScreenY() + m_OffsetY, 0.f };
        return {};
    }

    void GridMover::UpdateTransform(const glm::vec3& pos) const
    {
        if (auto* t = GetOwner()->GetGameComponent<dae::TransformComponent>())
            t->SetLocalPosition(pos);
    }
}
