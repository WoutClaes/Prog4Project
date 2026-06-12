#pragma once
#include "Components/GameComponent.h"
#include "Observer/IObserver.h"

namespace dae
{
    class SteamAchievementObserver final : public GameComponent, public IObserver
    {
    public:
        explicit SteamAchievementObserver(GameObject* pOwner)
            : GameComponent(pOwner) 
        {}

        ~SteamAchievementObserver() override = default;

        void OnNotify(GameObject* pGameObject, Event event) override;

        void Update() override {}
        void FixedUpdate() override {}
        void Render() const override {}

        SteamAchievementObserver(const SteamAchievementObserver&) = delete;
        SteamAchievementObserver& operator=(const SteamAchievementObserver&) = delete;
        SteamAchievementObserver(SteamAchievementObserver&&) = delete;
        SteamAchievementObserver& operator=(SteamAchievementObserver&&) = delete;
    };
}