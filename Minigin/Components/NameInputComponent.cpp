#include "NameInputComponent.h"
#include "Components/TextComponent.h"
#include "ScoreComponent.h"
#include "SceneManager.h"
#include <SDL3/SDL.h>

namespace dae
{
    NameInputComponent::NameInputComponent(GameObject* pOwner, TextComponent* textComp, int finalScore)
        : GameComponent(pOwner)
        , m_pTextComponent(textComp)
        , m_ScoreToSave(finalScore)
    {
        UpdateVisuals();
    }

    void NameInputComponent::Update()
    {

    }

    void NameInputComponent::ChangeLetter(int direction)
    {
        if (m_IsFinished) return;

        m_CurrentName[m_ActiveSlot] = static_cast<char>(m_CurrentName[m_ActiveSlot] + direction);

        if (m_CurrentName[m_ActiveSlot] > 'Z') m_CurrentName[m_ActiveSlot] = 'A';
        if (m_CurrentName[m_ActiveSlot] < 'A') m_CurrentName[m_ActiveSlot] = 'Z';

        UpdateVisuals();
    }

    void NameInputComponent::NextSlot()
    {
        if (m_IsFinished) return;

        m_ActiveSlot++;

        if (m_ActiveSlot > 2)
        {
            m_IsFinished = true;
            SaveAndContinue();
        }
        else
        {
            UpdateVisuals();
        }
    }

    void NameInputComponent::UpdateVisuals()
    {
        if (m_pTextComponent)
        {
            std::string display = m_CurrentName;
            display.insert(m_ActiveSlot + 1, "<");
            m_pTextComponent->SetText(display);
        }
    }

    void NameInputComponent::SaveAndContinue()
    {
        ScoreComponent::SaveCurrentScore(m_CurrentName, m_ScoreToSave);

        if (m_OnCompleteCallback)
        {
            m_OnCompleteCallback();
        }
    }
}