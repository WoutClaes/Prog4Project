#pragma once
#include "Components/GameComponent.h"
#include <memory>

namespace dae { class Texture2D; }

namespace qbert
{
    class DiskRenderComponent final : public dae::GameComponent
    {
    public:
        explicit DiskRenderComponent(dae::GameObject* pOwner);
        ~DiskRenderComponent() override = default;

        void Update() override;
        void FixedUpdate() override {}
        void Render() const override;

    private:
        std::shared_ptr<dae::Texture2D> m_Texture{};

        float m_AnimTimer{ 0.f };
        int m_CurrentFrame{ 0 };

        static constexpr float AnimSpeed = 0.15f;
        static constexpr int MaxFrames = 4;
        static constexpr float SrcW = 16.f;
        static constexpr float SrcH = 16.f;
        static constexpr float Scale = 3.f;
    };
}