#pragma once
#include "GameComponent.h"
#include "Observer/Subject.h"
#include <string>
#include <vector>

namespace dae
{
    struct HighScoreEntry
    {
        std::string name;
        int score{ 0 };
    };

    class ScoreComponent final : public GameComponent, public Subject
    {
    public:
        explicit ScoreComponent(GameObject* pOwner);
        ~ScoreComponent() override = default;

        void Update()      override {}
        void FixedUpdate() override {}
        void Render() const override {}

        void AddPoints(int amount);
        int GetScore() const { return m_Score; }

        static void SaveCurrentScore(const std::string& playerName, int score);
        static std::vector<HighScoreEntry> LoadHighScores();

        ScoreComponent(const ScoreComponent&) = delete;
        ScoreComponent& operator=(const ScoreComponent&) = delete;
        ScoreComponent(ScoreComponent&&) = delete;
        ScoreComponent& operator=(ScoreComponent&&) = delete;

    private:
        int  m_Score{ 0 };
        bool m_WinAchievementGranted{ false };

        static constexpr int WinScore{ 500 };
    };
}
