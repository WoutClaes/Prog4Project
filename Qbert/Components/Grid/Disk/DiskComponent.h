#pragma once
#include "Components/GameComponent.h"
#include <glm/glm.hpp>
#include <GameObject.h>

namespace qbert
{
    class GridMover;

    class DiskComponent final : public dae::GameComponent
    {
    public:
        DiskComponent(dae::GameObject* pOwner, dae::GameObject* pQbertObj, GridMover* pQbertMover, int diskRow, int diskCol, const glm::vec3& topPos);
        ~DiskComponent() override = default;

        void Update() override;

        int GetRow() const { return m_Row; }
        int GetCol() const { return m_Col; }

    private:
        dae::GameObject* m_pQbertObj{ nullptr };
        GridMover* m_pQbertMover{ nullptr };
        int m_Row{ 0 };
        int m_Col{ 0 };
        glm::vec3 m_TopPos{};

        bool m_IsActive{ true };
        bool m_IsMoving{ false };
        float m_MoveTimer{ 0.f };

        glm::vec3 m_StartPos{};

        glm::vec3 m_QbertOffset{};
    };
}
