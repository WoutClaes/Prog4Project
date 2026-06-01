#pragma once
#include "GameMode.h"
#include <string>

namespace dae { class Scene; }
namespace qbert { class CubeGrid; }

namespace qbert
{
    class LevelLoader final
    {
    public:
        static bool Load(int levelIndex, GameMode mode, dae::Scene& scene);

    private:
        static std::string LevelPath(int levelIndex);
    };
}
