#pragma once
#include "GameMode.h"
#include <string>
#include <functional>
#include "Singleton.h"
#include "Components/ScoreComponent.h"

namespace qbert
{
    class GameManager final
    {
    public:
        static GameManager& GetInstance();

        void StartGame(GameMode mode, int startLevel = 0);

        void Update();

        void LoadLevel(int levelIndex, int stageIndex);
        void ReloadCurrentLevel();
        void LoadNextLevel();

        void OnLevelComplete();
        void OnPlayerDied(dae::GameObject* pPlayerObj);
        void OnGameOver();
        void OnGameWin();

        int GetLives() const { return m_Lives; }
        int GetScore() const { return m_Score; }
        int GetLevel() const { return m_CurrentLevel; }
        int GetRound() const { return m_CurrentRound; }
        GameMode GetMode() const { return m_Mode; }
        bool IsGameOver() const { return m_GameOver; }
        bool IsLevelComplete() const { return m_LevelCompleteTimer > 0.0f; }

        void SetBonus(int bonus) { m_Bonus = bonus; }

        void AddScore(int amount);

        std::function<void(int levelIndex, int stageIndex, GameMode mode)> OnLoadLevel{};

        int m_Bonus{ 0 };

    private:
        GameManager() = default;

        bool m_PendingReload{ false };
        bool m_PendingNext{ false };
        bool m_PendingGameOver{ false };

        int m_Lives{ 4 };
        int m_Score{ 0 };
        int m_CurrentLevel{ 0 };
        int m_CurrentRound{ 0 };
        GameMode m_Mode { GameMode::SinglePlayer };
        bool m_GameOver { false };

        static constexpr int MaxLives { 4 };
        static constexpr int MaxLevels{ 3 };
        static constexpr int MaxStages{ 4 };
        static constexpr float m_Timer{ 3.f };

        int m_LevelStartScore{ 0 };
        float m_EndScreenTimer{ 0.0f };
        bool m_PendingHighScore{ false };

        float m_LevelCompleteTimer{ 0.0f };
        float m_DeathTimer{ 0.0f };

        int m_LastTransitionLevel{ -1 };
    };
}
