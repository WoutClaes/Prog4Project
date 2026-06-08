#pragma once
#include "Components/GameComponent.h"
#include <glm/glm.hpp>
#include <GameObject.h>
#include <vector>

namespace qbert
{
    class GridMover;

    class DiskComponent final : public dae::GameComponent
    {
    public:
        DiskComponent(dae::GameObject* pOwner, const std::vector<dae::GameObject*>& players, int diskRow, int diskCol, const glm::vec3& topPos);
        ~DiskComponent() override = default;

        void Update() override;

        int GetRow() const { return m_Row; }
        int GetCol() const { return m_Col; }

    private:
        std::vector<dae::GameObject*> m_Players{};

        dae::GameObject* m_pRidingObj{ nullptr };
        GridMover* m_pRidingMover{ nullptr };

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
