#pragma once
#include "Cube.h"
#include <array>

namespace qbert
{
    class CubeGrid final
    {
    public:
        static constexpr int   Rows       = 7;
        static constexpr int   TotalCubes = 28;

        // Source tile size from spritesheet (192x96, 6x3 = 32x32 per tile)
        static constexpr float TileW = 32.f;
        static constexpr float TileH = 32.f;

        // scale=1 uses raw 32x32, scale=2 uses 64x64 etc.
        explicit CubeGrid(float originX, float originY, float scale = 2.f);

        Cube*       GetCube(int row, int col);
        const Cube* GetCube(int row, int col) const;

        static bool IsValid(int row, int col);

        int  CountTargetCubes() const;
        bool IsComplete()       const;

        void Reset();
        void SetStepsToTarget(int steps);

        float GetScale() const { return m_Scale; }

        const std::array<Cube, TotalCubes>& GetCubes() const { return m_Cubes; }

    private:
        static int Index(int row, int col);

        std::array<Cube, TotalCubes> m_Cubes;
        float m_OriginX;
        float m_OriginY;
        float m_Scale;
    };
}
