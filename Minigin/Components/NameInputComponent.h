#pragma once
#include "GameComponent.h"
#include <string>
#include <functional>

namespace dae
{
    class TextComponent;

    class NameInputComponent final : public GameComponent
    {
    public:
        NameInputComponent(GameObject* pOwner, TextComponent* textComp, int finalScore);
        ~NameInputComponent() override = default;

        void Update() override;
        void FixedUpdate() override {}
        void Render() const override {}

        void ChangeLetter(int direction); 
        void NextSlot();

        void SetOnCompleteCallback(std::function<void()> callback) { m_OnCompleteCallback = std::move(callback); }

        NameInputComponent(const NameInputComponent&) = delete;
        NameInputComponent& operator=(const NameInputComponent&) = delete;
        NameInputComponent(NameInputComponent&&) = delete;
        NameInputComponent& operator=(NameInputComponent&&) = delete;

    private:
        void UpdateVisuals();
        void SaveAndContinue();

        dae::TextComponent* m_pTextComponent{ nullptr };
        std::string m_CurrentName{ "AAA" };
        int m_ActiveSlot{ 0 };
        int m_ScoreToSave{ 0 };
        bool m_IsFinished{ false };

        std::function<void()> m_OnCompleteCallback{};
    };
}