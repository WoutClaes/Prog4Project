#pragma once
#include "Components/GameComponent.h"
#include "Grid/CubeGrid.h"
#include "Sound/ISoundSystem.h"

namespace qbert
{
    // Directions Qbert can jump
    enum class JumpDirection
    {
        DownLeft,
        DownRight,
        UpLeft,
        UpRight
    };

    class QbertComponent final : public dae::GameComponent
    {
    public:
        explicit QbertComponent(dae::GameObject* pOwner, CubeGrid* pGrid, int row = 0, int col = 0);
        ~QbertComponent() override = default;

        void Update()      override;
        void FixedUpdate() override {}
        void Render() const override {}

        void RequestJump(JumpDirection dir);

        int GetRow() const { return m_Row; }
        int GetCol() const { return m_Col; }

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

        float m_JumpTimer{ 0.f };
        static constexpr float JumpDuration{ 0.15f };
        bool m_IsJumping{ false };

        dae::SoundId m_JumpSoundId{ dae::INVALID_SOUND_ID };
    };
}
