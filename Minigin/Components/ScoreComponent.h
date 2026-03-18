#pragma once
#include "GameComponent.h"
#include "Observer/Subject.h"

namespace dae
{
    class ScoreComponent final : public GameComponent, public Subject
    {
    public:
        explicit ScoreComponent(GameObject* pOwner);
        ~ScoreComponent() override = default;

        void Update()      override {}
        void FixedUpdate() override {}
        void Render() const override {}

        // Called by AddPointsCommand
        void AddPoints(int amount);

        int GetScore() const { return m_Score; }

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
