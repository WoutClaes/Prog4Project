#pragma once
#include <string>

namespace dae
{
    using SoundId = unsigned short;
    constexpr SoundId INVALID_SOUND_ID = 0;

    class ISoundSystem
    {
    public:
        virtual ~ISoundSystem() = default;

        virtual SoundId RegisterSound(const std::string& filename) = 0;

        virtual void Play(SoundId id, float volume = 1.f) = 0;

        virtual void SetMuted(bool muted) = 0;
        virtual bool IsMuted() const = 0;

        virtual void SetVolume(float volume) = 0;
        virtual float GetVolume() const = 0;
    };

    class NullSoundSystem final : public ISoundSystem
    {
    public:
        SoundId RegisterSound(const std::string&) override { return INVALID_SOUND_ID; }
        void Play(SoundId, float) override {}
        void SetMuted(bool) override {}
        bool IsMuted() const override { return false; }
        void SetVolume(float) override {}
        float GetVolume() const override { return 1.0f; }
    };
}
