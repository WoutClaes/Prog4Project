#include "GridRenderComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "Grid/CubeGrid.h"
#include "Grid/Cube.h"

namespace qbert
{
    GridRenderComponent::GridRenderComponent(dae::GameObject* pOwner, CubeGrid* pGrid)
        : GameComponent(pOwner)
        , m_pGrid(pGrid)
    {
        m_Texture = dae::ResourceManager::GetInstance().LoadTexture("Qbert Cubes.png");
    }

    void GridRenderComponent::Render() const
    {
        if (!m_pGrid || !m_Texture)
            return;

        for (const auto& cube : m_pGrid->GetCubes())
            RenderCube(cube);
    }

    void GridRenderComponent::RenderCube(const Cube& cube) const
    {
        const int   col  = ColorColForState(cube.GetState());
        const float srcX = col * TileW;
        const float srcY = 0.f;

        const SDL_FRect src{ srcX, srcY, TileW, TileH };

        dae::Renderer::GetInstance().RenderTexture(
            *m_Texture,
            cube.GetScreenX(),
            cube.GetScreenY(),
            src,
            TileW * Scale,
            TileH * Scale
        );
    }

    int GridRenderComponent::ColorColForState(CubeColorState state) const
    {
        switch (state)
        {
        case CubeColorState::Intermediate: return m_IntermediateCol;
        case CubeColorState::Target:       return m_TargetCol;
        case CubeColorState::Base:
        default:                           return m_BaseCol;
        }
    }
}
