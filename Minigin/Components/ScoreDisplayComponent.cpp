#include "ScoreDisplayComponent.h"
#include "TextComponent.h"
#include "ScoreComponent.h"
#include "GameObject.h"
#include <string>

namespace dae
{
    ScoreDisplayComponent::ScoreDisplayComponent(GameObject* pOwner)
        : GameComponent(pOwner)
    {}

    void ScoreDisplayComponent::OnNotify(GameObject* pGameObject, Event event)
    {
        if (event != Events::PointsChanged && event != Events::GameWon) return;

        auto* score = pGameObject->GetGameComponent<ScoreComponent>();
        if (!score) return;

        auto* text = GetOwner()->GetGameComponent<TextComponent>();
        if (!text) return;

        text->SetText("Score: " + std::to_string(score->GetScore()));
    }
}
