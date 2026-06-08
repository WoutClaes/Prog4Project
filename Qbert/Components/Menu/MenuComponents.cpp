#include "MenuComponents.h"
#include "GameTime.h"
#include "Components/TransformComponent.h"
#include "GameObject.h"

namespace qbert
{
    ScreenTimerComponent::ScreenTimerComponent(dae::GameObject* pOwner, float duration, std::function<void()> onComplete)
        : dae::GameComponent(pOwner), m_Timer(duration), m_OnComplete(onComplete)
    {}

    void ScreenTimerComponent::Update()
    {
        if (m_IsComplete) return;

        m_Timer -= dae::GameTime::GetInstance().GetDeltaTime();
        if (m_Timer <= 0.f)
        {
            m_IsComplete = true;
            if (m_OnComplete) m_OnComplete();
        }
    }

    MenuSelectorComponent::MenuSelectorComponent(dae::GameObject* pOwner)
        : dae::GameComponent(pOwner)
    {}

    void MenuSelectorComponent::AddOption(const glm::vec3& pos, std::function<void()> callback)
    {
        m_Options.push_back({ pos, callback });
    }

    void MenuSelectorComponent::Move(int direction)
    {
        if (m_Options.empty()) return;

        m_SelectedIndex = (m_SelectedIndex + direction + m_Options.size()) % m_Options.size();

        GetOwner()->GetGameComponent<dae::TransformComponent>()->SetPosition(
            m_Options[m_SelectedIndex].position.x,
            m_Options[m_SelectedIndex].position.y
        );
    }

    void MenuSelectorComponent::Select()
    {
        if (!m_Options.empty()) m_Options[m_SelectedIndex].callback();
    }
}
