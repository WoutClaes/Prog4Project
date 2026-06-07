#pragma once
#include "Components/GameComponent.h"
#include "Grid/CubeGrid.h"
#include <functional>
#include <glm/glm.hpp>
#include <vector>

namespace qbert
{
    class GridMover;

    struct DiskData
    {
        int row{ 0 };
        int col{ 0 };
        glm::vec3 screenPos;
    };

    class QbertComponent final : public dae::GameComponent
    {
    public:
        explicit QbertComponent(dae::GameObject* pOwner, CubeGrid* pGrid);
        ~QbertComponent() override = default;

        void Update() override {}
        void FixedUpdate() override {}
        void Render() const override {}

        void RequestJump(JumpDirection dir);

        void Die();

        void AddDisk(int row, int col, const glm::vec3& screenPos);

        int GetRow() const;
        int GetCol() const;

        std::function<void(JumpDirection)> OnJumpStarted{};
        std::function<void()> OnLanded{};

        QbertComponent(const QbertComponent&) = delete;
        QbertComponent& operator=(const QbertComponent&) = delete;
        QbertComponent(QbertComponent&&) = delete;
        QbertComponent& operator=(QbertComponent&&) = delete;

        bool IsRidingDisk() const { return m_IsRidingDisk; }
        void SetRidingDisk(bool riding, int diskRow = 0, int diskCol = 0)
        {
            m_IsRidingDisk = riding;
            m_DiskRow = diskRow;
            m_DiskCol = diskCol;
        }

        int GetDiskRow() const { return m_DiskRow; }
        int GetDiskCol() const { return m_DiskCol; }

    private:
        void Land();

        CubeGrid* m_pGrid{ nullptr };
        GridMover* m_pMover{ nullptr };

        std::vector<DiskData> m_Disks{};

        static constexpr int DeltaRow[4] = { 1,  1, -1, -1 };
        static constexpr int DeltaCol[4] = { 0,  1, -1,  0 };

        bool m_IsRidingDisk{ false };
        int m_DiskRow{ 0 };
        int m_DiskCol{ 0 };
    };
}
