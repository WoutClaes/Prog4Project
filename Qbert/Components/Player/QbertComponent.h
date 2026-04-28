#pragma once
#include "Components/GameComponent.h"
#include "Grid/CubeGrid.h"

namespace qbert
{
    // Directions Qbert can jump
    enum class JumpDirection
    {
        DownLeft,   // row+1, col
        DownRight,  // row+1, col+1
        UpLeft,     // row-1, col-1
        UpRight     // row-1, col
    };

    class QbertComponent final : public dae::GameComponent
    {
    public:
        explicit QbertComponent(dae::GameObject* pOwner, CubeGrid* pGrid, int row = 0, int col = 0);
        ~QbertComponent() override = default;

        void Update()      override;
        void FixedUpdate() override {}
        void Render() const override {}

        // Called by JumpCommand
        void RequestJump(JumpDirection dir);

        int GetRow() const { return m_Row; }
        int GetCol() const { return m_Col; }

        // Returns false if jump would leave the pyramid
        bool CanJump(JumpDirection dir) const;

        QbertComponent(const QbertComponent&) = delete;
        QbertComponent& operator=(const QbertComponent&) = delete;
        QbertComponent(QbertComponent&&) = delete;
        QbertComponent& operator=(QbertComponent&&) = delete;

    private:
        void Land();
        void UpdateScreenPosition();

        CubeGrid* m_pGrid{ nullptr };

        int  m_Row{ 0 };
        int  m_Col{ 0 };

        bool m_JumpPending{ false };
        JumpDirection m_PendingDir{ JumpDirection::DownLeft };

        // Simple jump animation timer
        float m_JumpTimer{ 0.f };
        static constexpr float JumpDuration{ 0.15f };
        bool m_IsJumping{ false };
    };
}
