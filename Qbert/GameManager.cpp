#include "GameManager.h"
#include "SceneManager.h"
#include <SDL3/SDL.h>

namespace qbert
{
    GameManager& GameManager::GetInstance()
    {
        static GameManager instance;
        return instance;
    }

    void GameManager::StartGame(GameMode mode, int startLevel)
    {
        m_Mode         = mode;
        m_Lives        = MaxLives;
        m_Score        = 0;
        m_GameOver     = false;
        m_CurrentLevel = startLevel;

        LoadLevel(m_CurrentLevel);
    }

    void GameManager::Update()
    {
        if (m_PendingGameOver)
        {
            m_PendingGameOver = false;
            OnGameOver();
            SDL_Log("GameManager: game over! Score: %d", m_Score);
            return;
        }
        if (m_PendingReload)
        {
            m_PendingReload = false;
            ReloadCurrentLevel();
            return;
        }
        if (m_PendingNext)
        {
            m_PendingNext = false;
            LoadNextLevel();
        }
    }

    void GameManager::LoadLevel(int levelIndex)
    {
        m_CurrentLevel = levelIndex;

        dae::SceneManager::GetInstance().QueueAction([this]()
            {
                dae::SceneManager::GetInstance().RemoveAllScenes();

                dae::SceneManager::GetInstance().CreateScene();

                if (OnLoadLevel)
                    OnLoadLevel(m_CurrentLevel, m_Mode);
                else
                    SDL_Log("GameManager: OnLoadLevel not set!");
            });
    }

    void GameManager::ReloadCurrentLevel()
    {
        LoadLevel(m_CurrentLevel);
    }

    void GameManager::LoadNextLevel()
    {
        const int next = m_CurrentLevel + 1;
        if (next >= MaxLevels)
        {
            SDL_Log("GameManager: all levels complete!");
            return;
        }
        LoadLevel(next);
    }

    void GameManager::OnLevelComplete()
    {
        m_PendingNext = true;
    }

    void GameManager::OnPlayerDied()
    {
        --m_Lives;
        SDL_Log("GameManager: player died, lives remaining: %d", m_Lives);
        if (m_Lives < 0)
            m_PendingGameOver = true;
        else
            m_PendingReload = true;
    }

    void GameManager::OnGameOver()
    {
        m_GameOver = true;
        SDL_Log("GameManager: game over! Final score: %d", m_Score);
    }

    void GameManager::AddScore(int amount)
    {
        m_Score += amount;
    }
}
