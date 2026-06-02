#pragma once
#include "Commands/Command.h"
#include "ServiceLocator.h"
#include <algorithm>

namespace qbert
{
    class ToggleMuteCommand final : public dae::Command
    {
    public:
        void Execute() override
        {
            auto& soundSystem = dae::ServiceLocator::GetSoundSystem();
            soundSystem.SetMuted(!soundSystem.IsMuted());
        }
    };

    class VolumeUpCommand final : public dae::Command
    {
    public:
        void Execute() override
        {
            auto& soundSystem = dae::ServiceLocator::GetSoundSystem();
            float newVolume = std::clamp(soundSystem.GetVolume() + 0.1f, 0.0f, 1.0f);
            soundSystem.SetVolume(newVolume);
        }
    };

    class VolumeDownCommand final : public dae::Command
    {
    public:
        void Execute() override
        {
            auto& soundSystem = dae::ServiceLocator::GetSoundSystem();
            float newVolume = std::clamp(soundSystem.GetVolume() - 0.1f, 0.0f, 1.0f);
            soundSystem.SetVolume(newVolume);
        }
    };
}