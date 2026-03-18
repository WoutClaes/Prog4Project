#pragma once
#include "GameComponent.h"
#include "Observer/IObserver.h"

namespace dae
{
    class TextComponent;

    class ScoreDisplayComponent final : public GameComponent, public IObserver
    {
    public:
        explicit ScoreDisplayComponent(GameObject* pOwner);
        ~ScoreDisplayComponent() override = default;

        void Update()      override {}
        void FixedUpdate() override {}
        void Render() const override {}

        // IObserver
        void OnNotify(GameObject* pGameObject, Event event) override;

        ScoreDisplayComponent(const ScoreDisplayComponent&) = delete;
        ScoreDisplayComponent& operator=(const ScoreDisplayComponent&) = delete;
        ScoreDisplayComponent(ScoreDisplayComponent&&) = delete;
        ScoreDisplayComponent& operator=(ScoreDisplayComponent&&) = delete;
    };
}
