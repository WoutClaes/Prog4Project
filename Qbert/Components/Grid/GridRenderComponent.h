#pragma once
#include "Components/GameComponent.h"
#include "Grid/CubeGrid.h"
#include <memory>

namespace dae { class Texture2D; }

namespace qbert
{
    class GridRenderComponent final : public dae::GameComponent
    {
    public:
        explicit GridRenderComponent(dae::GameObject* pOwner, CubeGrid* pGrid);
        ~GridRenderComponent() override = default;

        void Update()      override {}
        void FixedUpdate() override {}
        void Render() const override;

        void SetBaseColorIndex(int col)         { m_BaseCol = col; }
        void SetIntermediateColorIndex(int col) { m_IntermediateCol = col; }
        void SetTargetColorIndex(int col)       { m_TargetCol = col; }

        GridRenderComponent(const GridRenderComponent&) = delete;
        GridRenderComponent& operator=(const GridRenderComponent&) = delete;
        GridRenderComponent(GridRenderComponent&&) = delete;
        GridRenderComponent& operator=(GridRenderComponent&&) = delete;

    private:
        void RenderCube(const Cube& cube) const;
        int  ColorColForState(CubeColorState state) const;

        CubeGrid* m_pGrid{ nullptr };
        std::shared_ptr<dae::Texture2D> m_Texture{};

        static constexpr float TileW  = 32.f;
        static constexpr float TileH  = 32.f;
        static constexpr float Scale  = 2.f;   // renders at 64x64, adjust to taste

        int m_BaseCol{ 1 };
        int m_IntermediateCol{ 2 };
        int m_TargetCol{ 3 };
    };
}
