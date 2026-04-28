#pragma once
#include "ISoundSystem.h"
#include <memory>

namespace dae
{
    class ServiceLocator final
    {
    public:
        static ISoundSystem& GetSoundSystem()
        {
            return *m_pSoundSystem;
        }

        static void RegisterSoundSystem(std::unique_ptr<ISoundSystem> system)
        {
            m_pSoundSystem = system
                ? std::move(system)
                : std::make_unique<NullSoundSystem>();
        }

        ServiceLocator() = delete;

    private:
        static std::unique_ptr<ISoundSystem> m_pSoundSystem;
    };
}
