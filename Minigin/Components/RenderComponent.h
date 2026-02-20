#pragma once
#include "GameComponent.h"
#include <memory>
#include<string>

namespace dae
{
	class Texture2D;

	class RenderComponent : public GameComponent
	{
	public:
		explicit RenderComponent(GameObject* pOwner)
			: GameComponent(pOwner)
		{};

		void SetTexture(const std::string& filename);
		void Render() const override;

	private:
		std::shared_ptr<Texture2D> m_Texture{};
	};
}