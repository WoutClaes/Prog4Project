#pragma once
#include "GameComponent.h"
#include <memory>
#include <string>
#include <SDL3/SDL.h>

namespace dae
{
    class Texture2D;

    class SpriteComponent final : public GameComponent
    {
    public:
        explicit SpriteComponent(GameObject* pOwner);
        ~SpriteComponent() override = default;

        void Update() override {}
        void FixedUpdate() override {}
        void Render() const override;

        void SetSpriteSheet(const std::string& filename);
        void SetSpriteSheet(std::shared_ptr<Texture2D> texture);

        void SetSourceRect(float x, float y, float w, float h);

        void SetDestSize(float w, float h);
        void ClearDestSize();

        SpriteComponent(const SpriteComponent&) = delete;
        SpriteComponent& operator=(const SpriteComponent&) = delete;
        SpriteComponent(SpriteComponent&&) = delete;
        SpriteComponent& operator=(SpriteComponent&&) = delete;

    private:
        std::shared_ptr<Texture2D> m_Texture{};
        SDL_FRect m_SrcRect{ 0.f, 0.f, 0.f, 0.f };
        bool m_HasDestSize{ false };
        float m_DestW{ 0.f };
        float m_DestH{ 0.f };
    };
}
