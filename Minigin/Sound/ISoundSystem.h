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

        // Register a sound file path, returns an ID for Play calls.
        virtual SoundId RegisterSound(const std::string& filename) = 0;

        // Post a play request onto the queue -- processed on worker thread.
        virtual void Play(SoundId id, float volume = 1.f) = 0;

        virtual void SetMuted(bool muted) = 0;
        virtual bool IsMuted() const = 0;
    };

    // Does nothing -- default before a real system is registered.
    class NullSoundSystem final : public ISoundSystem
    {
    public:
        SoundId RegisterSound(const std::string&) override { return INVALID_SOUND_ID; }
        void    Play(SoundId, float)               override {}
        void    SetMuted(bool)                     override {}
        bool    IsMuted() const                    override { return false; }
    };
}
