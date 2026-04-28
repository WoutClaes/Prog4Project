#include "ServiceLocator.h"

namespace dae
{
    std::unique_ptr<ISoundSystem> ServiceLocator::m_pSoundSystem =
        std::make_unique<NullSoundSystem>();
}
