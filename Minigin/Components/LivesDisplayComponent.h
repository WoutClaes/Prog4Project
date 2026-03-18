#pragma once
#include "GameComponent.h"
#include "Observer/IObserver.h"

namespace dae
{
    class TextComponent;

    class LivesDisplayComponent final : public GameComponent, public IObserver
    {
    public:
        explicit LivesDisplayComponent(GameObject* pOwner);
        ~LivesDisplayComponent() override = default;

        void Update()      override {}
        void FixedUpdate() override {}
        void Render() const override {}

        // IObserver
        void OnNotify(GameObject* pGameObject, Event event) override;

        LivesDisplayComponent(const LivesDisplayComponent&) = delete;
        LivesDisplayComponent& operator=(const LivesDisplayComponent&) = delete;
        LivesDisplayComponent(LivesDisplayComponent&&) = delete;
        LivesDisplayComponent& operator=(LivesDisplayComponent&&) = delete;
    };
}
