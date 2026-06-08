#pragma once
#include "Components/GameComponent.h"
#include <vector>
#include <functional>
#include <glm/vec3.hpp>

namespace qbert
{
    // --- Timer Component for Auto-Advancing Screens ---
    class ScreenTimerComponent final : public dae::GameComponent
    {
    public:
        ScreenTimerComponent(dae::GameObject* pOwner, float duration, std::function<void()> onComplete);
        void Update() override;

    private:
        float m_Timer;
        bool m_IsComplete{ false };
        std::function<void()> m_OnComplete;
    };

    // --- Menu Selection Component ---
    class MenuSelectorComponent final : public dae::GameComponent
    {
    public:
        MenuSelectorComponent(dae::GameObject* pOwner);

        void AddOption(const glm::vec3& pos, std::function<void()> callback);
        void Move(int direction);
        void Select();

    private:
        struct MenuOption {
            glm::vec3 position;
            std::function<void()> callback;
        };
        std::vector<MenuOption> m_Options;
        size_t m_SelectedIndex{ 0 };
    };
}
