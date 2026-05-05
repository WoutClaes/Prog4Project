#pragma once
#include "Components/GameComponent.h"
#include "CoilyComponent.h"
#include <memory>

namespace dae { class Texture2D; }

namespace qbert
{
    class CoilyRenderComponent final : public dae::GameComponent
    {
    public:
        explicit CoilyRenderComponent(dae::GameObject* pOwner,
                                      CoilyComponent* pCoily);
        ~CoilyRenderComponent() override = default;

        void Update()      override {}
        void FixedUpdate() override {}
        void Render() const override;

        CoilyRenderComponent(const CoilyRenderComponent&) = delete;
        CoilyRenderComponent& operator=(const CoilyRenderComponent&) = delete;
        CoilyRenderComponent(CoilyRenderComponent&&) = delete;
        CoilyRenderComponent& operator=(CoilyRenderComponent&&) = delete;

    private:
        CoilyComponent* m_pCoily{ nullptr };
        std::shared_ptr<dae::Texture2D> m_Texture{};

        static constexpr float SrcW  = 16.f;
        static constexpr float SrcH  = 32.f;
        static constexpr float Scale = 3.f;
    };
}
