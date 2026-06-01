#pragma once
#include "GameMode.h"
#include <string>
#include <functional>
#include "Singleton.h"

namespace qbert
{
    class GameManager final
    {
    public:
        static GameManager& GetInstance();

        void StartGame(GameMode mode, int startLevel = 0);

        void Update();

        void LoadLevel(int levelIndex);
        void ReloadCurrentLevel();
        void LoadNextLevel();

        void OnLevelComplete();
        void OnPlayerDied();
        void OnGameOver();

        int      GetLives()        const { return m_Lives; }
        int      GetScore()        const { return m_Score; }
        int      GetLevel()        const { return m_CurrentLevel; }
        GameMode GetMode()         const { return m_Mode; }
        bool     IsGameOver()      const { return m_GameOver; }

        void AddScore(int amount);

        std::function<void(int levelIndex, GameMode mode)> OnLoadLevel{};

    private:
        GameManager() = default;

        bool m_PendingReload{ false };
        bool m_PendingNext{ false };
        bool m_PendingGameOver{ false };

        int      m_Lives        { 3 };
        int      m_Score        { 0 };
        int      m_CurrentLevel { 0 };
        GameMode m_Mode         { GameMode::SinglePlayer };
        bool     m_GameOver     { false };

        static constexpr int MaxLives { 3 };
        static constexpr int MaxLevels{ 3 };
    };
}
