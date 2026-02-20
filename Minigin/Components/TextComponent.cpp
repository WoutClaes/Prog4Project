#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "TextComponent.h"
#include "TransformComponent.h"
#include "Renderer.h"
#include "Font.h"
#include "Texture2D.h"
#include "GameObject.h"

dae::TextComponent::TextComponent(GameObject* pOwner, const std::string& text, std::shared_ptr<Font> font, const SDL_Color& color)
	: GameComponent(pOwner)
	, m_NeedsUpdate(true)
	, m_Text(text)
	,m_Color(color)
	,m_Font(std::move(font))
{}

void dae::TextComponent::Update()
{
    if (m_NeedsUpdate)
    {
        if (m_Text.empty())
        {
            m_NeedsUpdate = false;
            return;
        }

        const auto surf = TTF_RenderText_Blended(m_Font->GetFont(), m_Text.c_str(), m_Text.length(), m_Color);
        if (surf == nullptr)
            throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());

        auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surf);
        if (texture == nullptr)
            throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());

        SDL_DestroySurface(surf);
        m_TextTexture = std::make_shared<Texture2D>(texture);
        m_NeedsUpdate = false;
    }
}

void dae::TextComponent::Render() const
{
    if (!m_TextTexture) return;

    auto* transform = GetOwner()->GetGameComponent<TransformComponent>();
    if (!transform) return;

    const auto& pos = transform->GetTransform().GetPosition();
    Renderer::GetInstance().RenderTexture(*m_TextTexture, pos.x, pos.y);
}

void dae::TextComponent::SetText(const std::string& text)
{
    m_Text = text;
    m_NeedsUpdate = true;
}

void dae::TextComponent::SetColor(const SDL_Color& color)
{
    m_Color = color;
    m_NeedsUpdate = true;
}
