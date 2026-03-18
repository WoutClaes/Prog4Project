#include "LivesDisplayComponent.h"
#include "TextComponent.h"
#include "HealthComponent.h"
#include "GameObject.h"
#include <string>

namespace dae
{
    LivesDisplayComponent::LivesDisplayComponent(GameObject* pOwner)
        : GameComponent(pOwner)
    {}

    void LivesDisplayComponent::OnNotify(GameObject* pGameObject, Event event)
    {
        if (event != Events::PlayerDied) return;

        auto* health = pGameObject->GetGameComponent<HealthComponent>();
        if (!health) return;

        auto* text = GetOwner()->GetGameComponent<TextComponent>();
        if (!text) return;

        text->SetText("# lives: " + std::to_string(health->GetLives()));
    }
}
