#pragma once
#include "Components/GameComponent.h"
#include "QbertComponent.h"
#include <memory>

namespace dae { class Texture2D; }

namespace qbert
{
    class QbertRenderComponent final : public dae::GameComponent
    {
    public:
        explicit QbertRenderComponent(dae::GameObject* pOwner, QbertComponent* pQbert, int playerIdx);
        ~QbertRenderComponent() override = default;

        void Update() override {}
        void FixedUpdate() override {}
        void Render() const override;

        void SetLastDirection(JumpDirection dir) { m_LastDir = dir; }

        QbertRenderComponent(const QbertRenderComponent&) = delete;
        QbertRenderComponent& operator=(const QbertRenderComponent&) = delete;
        QbertRenderComponent(QbertRenderComponent&&) = delete;
        QbertRenderComponent& operator=(QbertRenderComponent&&) = delete;

    private:
        std::shared_ptr<dae::Texture2D> m_Texture{};

        JumpDirection m_LastDir{ JumpDirection::DownLeft };

        static constexpr float SrcW = 17.f;
        static constexpr float SrcH = 16.f;
        static constexpr float Scale = 3.f;

        int m_PlayerIdx{ -1 };
    };
}
