#include "ScoreComponent.h"
#include "GameObject.h"
#include <fstream>
#include <algorithm>
#include <filesystem>

namespace dae
{
    ScoreComponent::ScoreComponent(GameObject* pOwner)
        : GameComponent(pOwner)
    {}

    void ScoreComponent::AddPoints(int amount)
    {
        m_Score += amount;
        Notify(GetOwner(), Events::PointsChanged);

        if (!m_WinAchievementGranted && m_Score >= WinScore)
        {
            m_WinAchievementGranted = true;
            Notify(GetOwner(), Events::GameWon);
        }
    }

    void ScoreComponent::SaveCurrentScore(const std::string& playerName, int score)
    {
        auto scores = LoadHighScores();
        scores.push_back({ playerName, score });

        std::sort(scores.begin(), scores.end(), [](const HighScoreEntry& a, const HighScoreEntry& b) {
            return a.score > b.score;
            });

        if (scores.size() > 15) scores.resize(15);

        std::filesystem::create_directories("Data/Scores");

        std::ofstream outFile("Data/Scores/highscores.txt");
        for (const auto& entry : scores)
        {
            outFile << entry.name << " " << entry.score << "\n";
        }
    }

    std::vector<HighScoreEntry> ScoreComponent::LoadHighScores()
    {
        std::vector<HighScoreEntry> scores;
        std::ifstream inFile("Data/Scores/highscores.txt");
        std::string name;
        int score;

        if (inFile.is_open())
        {
            while (inFile >> name >> score)
            {
                scores.push_back({ name, score });
            }
        }
        return scores;
    }
}
