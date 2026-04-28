#pragma once
#include "ISoundSystem.h"
#include <memory>

namespace dae
{
    // SDL3 native audio implementation.
    // SDL3 headers are ONLY in the .cpp -- never leak into this header (Pimpl).
    class SDLSoundSystem final : public ISoundSystem
    {
    public:
        SDLSoundSystem();
        ~SDLSoundSystem() override;

        SoundId RegisterSound(const std::string& filename) override;
        void    Play(SoundId id, float volume = 1.f) override;
        void    SetMuted(bool muted) override;
        bool    IsMuted() const override;

        SDLSoundSystem(const SDLSoundSystem&) = delete;
        SDLSoundSystem& operator=(const SDLSoundSystem&) = delete;
        SDLSoundSystem(SDLSoundSystem&&) = delete;
        SDLSoundSystem& operator=(SDLSoundSystem&&) = delete;

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };
}
