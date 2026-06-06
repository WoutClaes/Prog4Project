#pragma once
#include "GameMode.h"
#include <string>
#include <InputManager.h>

namespace dae { class Scene; }
namespace qbert { class CubeGrid; }

namespace qbert
{
    class LevelLoader final
    {
    public:
        static bool Load(int levelIndex, int stageIndex, GameMode mode, dae::Scene& scene);

        static void QueueLoadLevel(int levelIndex, int stageIndex, GameMode mode);
        static void QueueLoadHighScoreInput(int finalScore);

        static void QueueLoadEndScreen(bool isWin);

        static void QueueLoadHighScoreBoard();

    private:
        static std::string LevelPath(int levelIndex);
    };
}
