#pragma once
#include "Components/GameComponent.h"
#include "SlickSamComponent.h"
#include <memory>

namespace dae { class Texture2D; }

namespace qbert
{
    class SlickSamRenderComponent final : public dae::GameComponent
    {
    public:
        explicit SlickSamRenderComponent(dae::GameObject* pOwner, SlickSamComponent* pSlickSam);
        ~SlickSamRenderComponent() override = default;

        void Update() override {}
        void FixedUpdate() override {}
        void Render() const override;

        SlickSamRenderComponent(const SlickSamRenderComponent&) = delete;
        SlickSamRenderComponent& operator=(const SlickSamRenderComponent&) = delete;
        SlickSamRenderComponent(SlickSamRenderComponent&&) = delete;
        SlickSamRenderComponent& operator=(SlickSamRenderComponent&&) = delete;

    private:
        SlickSamComponent* m_pSlickSam{ nullptr };
        std::shared_ptr<dae::Texture2D> m_Texture{};

        static constexpr float TileW = 12.f;
        static constexpr float TileH = 16.f;
        static constexpr float Scale = 3.f;
    };
}
