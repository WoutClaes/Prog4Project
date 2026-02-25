#pragma once
#include "GameComponent.h"
#include <memory>
#include <string>

namespace dae
{
	class Texture2D;

	class RenderComponent final : public GameComponent
	{
	public:
		explicit RenderComponent(GameObject* owner)
			: GameComponent(owner) {
		}

		~RenderComponent() override = default;

		void SetTexture(const std::string& filename);
		void SetTexture(std::shared_ptr<Texture2D> texture);

		void Update()       override {}
		void FixedUpdate()  override {}
		void Render() const override;

		RenderComponent(const RenderComponent&) = delete;
		RenderComponent(RenderComponent&&) = delete;
		RenderComponent& operator=(const RenderComponent&) = delete;
		RenderComponent& operator=(RenderComponent&&) = delete;

	private:
		std::shared_ptr<Texture2D> m_texture{};
	};
}