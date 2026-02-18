#pragma once
#include <string>
#include <memory>
#include <SDL3/SDL.h>
#include <GameComponent.h>

namespace dae
{
	class Font;
	class Texture2D;

	class TextComponent final : public GameComponent
	{
	public:
		TextComponent(GameObject* pOwner, const std::string& text,
			std::shared_ptr<Font> font, const SDL_Color& color = { 255, 255, 255, 255 });
		~TextComponent() override = default;

		void Update() override;
		void Render() const override;

		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);

		TextComponent(const TextComponent&) = delete;
		TextComponent& operator=(const TextComponent&) = delete;
		TextComponent(TextComponent&&) = delete;
		TextComponent& operator=(TextComponent&&) = delete;

	private:
		bool m_NeedsUpdate{ true };
		std::string m_Text{};
		SDL_Color m_Color{ 255, 255, 255, 255 };
		std::shared_ptr<Font> m_Font{};
		std::shared_ptr<Texture2D> m_TextTexture{};
	};
}