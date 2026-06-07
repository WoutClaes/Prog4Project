#include "CubeGrid.h"

namespace qbert
{
    static std::array<Cube, CubeGrid::TotalCubes> BuildCubes()
    {
        std::array<Cube, CubeGrid::TotalCubes> cubes{};
        int idx = 0;
        for (int row = 0; row < CubeGrid::Rows; ++row)
            for (int col = 0; col <= row; ++col)
                cubes[idx++] = Cube(row, col);
        return cubes;
    }

    CubeGrid::CubeGrid(float originX, float originY, float scale)
        : m_Cubes(BuildCubes())
        , m_OriginX(originX)
        , m_OriginY(originY)
        , m_Scale(scale)
    {
        const float stepX = (TileW * scale) / 2.f;
        const float stepY = TileH * scale * 0.75f;

        for (auto& cube : m_Cubes)
        {
            const int   row = cube.GetRow();
            const int   col = cube.GetCol();
            const float x   = m_OriginX + col * TileW * scale - row * stepX;
            const float y   = m_OriginY + row * stepY;
            cube.SetScreenPos(x, y);
        }
    }

    Cube* CubeGrid::GetCube(int row, int col)
    {
        if (!IsValid(row, col)) return nullptr;
        return &m_Cubes[Index(row, col)];
    }

    const Cube* CubeGrid::GetCube(int row, int col) const
    {
        if (!IsValid(row, col)) return nullptr;
        return &m_Cubes[Index(row, col)];
    }

    bool CubeGrid::IsValid(int row, int col)
    {
        return row >= 0 && row < Rows && col >= 0 && col <= row;
    }

    int CubeGrid::CountTargetCubes() const
    {
        int count = 0;
        for (const auto& cube : m_Cubes)
            if (cube.IsTarget()) ++count;
        return count;
    }

    bool CubeGrid::IsComplete() const
    {
        return CountTargetCubes() == TotalCubes;
    }

    void CubeGrid::SetReverts(bool reverts)
    {
        m_Reverts = reverts;
        for (auto& cube : m_Cubes)
        {
            cube.m_Reverts = reverts;
        }
    }

    void CubeGrid::Reset()
    {
        for (auto& cube : m_Cubes)
            cube.Reset();
    }

    void CubeGrid::SetStepsToTarget(int steps)
    {
        for (auto& cube : m_Cubes)
            cube.SetStepsToTarget(steps);
    }

    int CubeGrid::Index(int row, int col)
    {
        return row * (row + 1) / 2 + col;
    }
}
