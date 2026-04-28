#pragma once
#include "Components/GameComponent.h"
#include "Grid/CubeGrid.h"

namespace qbert
{
    // Thin wrapper so CubeGrid can live as a GameComponent
    // and be owned by the scene's GameObject lifetime.
    class CubeGridComponent final : public dae::GameComponent
    {
    public:
        explicit CubeGridComponent(dae::GameObject* pOwner, float originX, float originY)
            : GameComponent(pOwner)
            , m_Grid(originX, originY)
        {}

        ~CubeGridComponent() override = default;

        void Update()      override {}
        void FixedUpdate() override {}
        void Render() const override {}

        CubeGrid*       GetGrid()       { return &m_Grid; }
        const CubeGrid* GetGrid() const { return &m_Grid; }

        CubeGridComponent(const CubeGridComponent&) = delete;
        CubeGridComponent& operator=(const CubeGridComponent&) = delete;
        CubeGridComponent(CubeGridComponent&&) = delete;
        CubeGridComponent& operator=(CubeGridComponent&&) = delete;

    private:
        CubeGrid m_Grid;
    };
}
