#include "GameManager.h"
#include "SceneManager.h"
#include <Components/ScoreComponent.h>
#include <SDL3/SDL_log.h>
#include "GameMode.h"
#include "LevelLoader.h"
#include "GameTime.h"
#include "GameObject.h"
#include "Scene.h"
#include "GameManagerUpdater.h"
#include "Sound/ServiceLocator.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"

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
        m_CurrentRound = 0;

        LoadLevel(m_CurrentLevel, m_CurrentRound);
    }

    void GameManager::Update()
    {
        if (m_EndScreenTimer > 0.f)
        {
            m_EndScreenTimer -= dae::GameTime::GetInstance().GetDeltaTime();
            if (m_EndScreenTimer <= 0.f)
            {
                if (m_PendingHighScore)
                {
                    m_PendingHighScore = false;
                    LevelLoader::QueueLoadHighScoreInput(m_Score);
                }
                else
                {
                    dae::SceneManager::GetInstance().QueueAction([]() {
                        dae::SceneManager::GetInstance().RemoveAllScenes();
                        dae::SceneManager::GetInstance().CreateScene();
                        });
                }
            }
            return;
        }

        if (m_LevelCompleteTimer > 0.f)
        {
            m_LevelCompleteTimer -= dae::GameTime::GetInstance().GetDeltaTime();
            if (m_LevelCompleteTimer <= 0.f)
            {
                m_PendingNext = true;
            }
            return;
        }

        if (m_DeathTimer > 0.f)
        {
            m_DeathTimer -= dae::GameTime::GetInstance().GetDeltaTime();
            if (m_DeathTimer <= 0.f)
            {
                if (m_PendingGameOver)
                {
                    m_PendingGameOver = false;
                    OnGameOver();
                    SDL_Log("GameManager: game over! Score: %d", m_Score);
                }
                else if (m_PendingReload)
                {
                    m_PendingReload = false;
                    ReloadCurrentLevel();
                }
            }
            return;
        }

        if (m_PendingNext)
        {
            m_PendingNext = false;
            LoadNextLevel();
        }
    }

    void GameManager::LoadLevel(int levelIndex, int stageIndex)
    {
        m_CurrentLevel = levelIndex;
        m_CurrentRound = stageIndex;
        m_LevelStartScore = m_Score;

        m_LevelCompleteTimer = 0.0f;
        m_PendingNext = false;

        LevelLoader::QueueLoadLevel(m_CurrentLevel, m_CurrentRound, m_Mode);
    }

    void GameManager::ReloadCurrentLevel()
    {
        LoadLevel(m_CurrentLevel, m_CurrentRound);
    }

    void GameManager::LoadNextLevel()
    {
        m_CurrentRound++;

        if (m_CurrentRound >= MaxStages)
        {
            m_CurrentRound = 0;
            m_CurrentLevel++;

            if (m_CurrentLevel > MaxLevels)
            {
                SDL_Log("GameManager: All levels and stages complete! Victory!");
                OnGameWin();
                return;
            }
        }

        LoadLevel(m_CurrentLevel, m_CurrentRound);
    }

    void GameManager::OnLevelComplete()
    {
        if (m_LevelCompleteTimer > 0.f || m_PendingNext)
        {
            return;
        }
        AddScore(m_Bonus);
        m_LevelCompleteTimer = m_Timer;

        dae::ServiceLocator::GetSoundSystem().Play(dae::ServiceLocator::GetSoundSystem().RegisterSound("Data/Sounds/Round Complete Tune.wav"), 1.f);

        auto& scene = dae::SceneManager::GetInstance().GetActiveScene();
        for (const auto& obj : scene.GetObjects())
        {
            if (!obj->HasGameComponent<GameManagerUpdater>())
            {
                obj->SetActive(false);
            }
        }
        
    }

    void GameManager::OnPlayerDied(dae::GameObject* pPlayerObj)
    {
        if (m_DeathTimer > 0.f) return;

        --m_Lives;
        SDL_Log("GameManager: player died, lives remaining: %d", m_Lives);

        if (m_Lives <= 0)
        {
            m_PendingGameOver = true;
        }
        else
        {
            m_Score = m_LevelStartScore;
            m_PendingReload = true;
        }

        m_DeathTimer = m_Timer;

        dae::ServiceLocator::GetSoundSystem().Play(dae::ServiceLocator::GetSoundSystem().RegisterSound("Data/Sounds/Swearing.wav"), 1.f);

        glm::vec3 playerPos{ 0.f, 0.f, 0.f };
        if (pPlayerObj && pPlayerObj->HasGameComponent<dae::TransformComponent>())
        {
            playerPos = pPlayerObj->GetGameComponent<dae::TransformComponent>()->GetWorldPosition();
        }

        auto& scene = dae::SceneManager::GetInstance().GetActiveScene();
        for (const auto& obj : scene.GetObjects())
        {
            if (!obj->HasGameComponent<GameManagerUpdater>())
            {
                obj->SetActive(false);
            }
        }

        dae::SceneManager::GetInstance().QueueAction([playerPos]() {
            auto& deferredScene = dae::SceneManager::GetInstance().GetActiveScene();

            auto swearObj = std::make_unique<dae::GameObject>();
            swearObj->AddGameComponent<dae::TransformComponent>()->SetPosition(playerPos.x, playerPos.y - 32.f);

            auto* sprite = swearObj->AddGameComponent<dae::SpriteComponent>();
            sprite->SetSpriteSheet("Qbert Curses.png");
            sprite->SetSourceRect(0.f, 0.f, 48.f, 25.f);
            sprite->SetDestSize(64.f, 32.f);

            swearObj->AddGameComponent<GameManagerUpdater>();
            deferredScene.Add(std::move(swearObj));
            });
    }

    void GameManager::OnGameOver()
    {
        m_GameOver = true;

        auto scores = dae::ScoreComponent::LoadHighScores();
        m_PendingHighScore = (scores.size() < 15) || (m_Score > scores.back().score);

        m_EndScreenTimer = m_Timer;

        LevelLoader::QueueLoadEndScreen(false);
    }

    void GameManager::OnGameWin()
    {
        m_GameOver = true;

        auto scores = dae::ScoreComponent::LoadHighScores();
        m_PendingHighScore = (scores.size() < 15) || (m_Score > scores.back().score);

        m_EndScreenTimer = m_Timer;

        LevelLoader::QueueLoadEndScreen(true);
    }

    void GameManager::AddScore(int amount)
    {
        m_Score += amount;
    }
}
