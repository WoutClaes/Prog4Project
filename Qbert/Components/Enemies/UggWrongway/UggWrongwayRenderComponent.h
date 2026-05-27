#pragma once
#include "Components/GameComponent.h"
#include "UggWrongwayComponent.h"
#include <memory>

namespace dae { class Texture2D; }

namespace qbert
{
    class UggWrongwayRenderComponent final : public dae::GameComponent
    {
    public:
        explicit UggWrongwayRenderComponent(dae::GameObject* pOwner, UggWrongwayComponent* pUggWrongway);
        ~UggWrongwayRenderComponent() override = default;

        void Update() override {}
        void FixedUpdate() override {}
        void Render() const override;

        UggWrongwayRenderComponent(const UggWrongwayRenderComponent&) = delete;
        UggWrongwayRenderComponent& operator=(const UggWrongwayRenderComponent&) = delete;
        UggWrongwayRenderComponent(UggWrongwayRenderComponent&&) = delete;
        UggWrongwayRenderComponent& operator=(UggWrongwayRenderComponent&&) = delete;

    private:
        UggWrongwayComponent* m_pUggWrongway{ nullptr };
        std::shared_ptr<dae::Texture2D> m_Texture{};

        static constexpr float TileW  = 16.f;
        static constexpr float TileH  = 16.f;
        static constexpr float Scale  = 3.f;
    };
}
