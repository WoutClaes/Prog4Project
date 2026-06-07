#pragma once
#include "Components/GameComponent.h"
#include "Grid/CubeGrid.h"
#include <functional>
#include <glm/glm.hpp>

namespace qbert
{
    class GridMover final : public dae::GameComponent
    {
    public:
        explicit GridMover(dae::GameObject* pOwner, CubeGrid* pGrid, int startRow, int startCol, float offsetX = 0.f, float offsetY = -20.f);
        ~GridMover() override = default;

        void Update() override;
        void FixedUpdate() override {}
        void Render() const override {}

        bool RequestJump(int newRow, int newCol);

        bool ForceJump(int newRow, int newCol, const glm::vec3& targetPos);
        bool JumpOff(int deltaRow, int deltaCol);
        void SetGridPosition(int row, int col, const glm::vec3& screenPos);

        bool IsJumping() const { return m_IsJumping; }
        int  GetRow() const { return m_Row; }
        int  GetCol() const { return m_Col; }

        void SetOffsetY(float offsetY) { m_OffsetY = offsetY; }

        float GetJumpProgress() const { return m_IsJumping ? m_JumpTimer / JumpDuration : 1.f; }

        std::function<void()> OnLanded{};

        static constexpr float JumpDuration{ 0.25f };

        GridMover(const GridMover&) = delete;
        GridMover& operator=(const GridMover&) = delete;
        GridMover(GridMover&&) = delete;
        GridMover& operator=(GridMover&&) = delete;

    private:
        glm::vec3 CubeScreenPos(int row, int col) const;
        void UpdateTransform(const glm::vec3& pos) const;

        CubeGrid* m_pGrid{ nullptr };

        int m_Row{ 0 };
        int m_Col{ 0 };

        bool m_IsJumping{ false };
        bool m_MidAirFired{ false };
        float m_JumpTimer{ 0.f };

        float m_OffsetX{ 0.f };
        float m_OffsetY{ -20.f };

        glm::vec3 m_StartPos{};
        glm::vec3 m_TargetPos{};
    };
}
