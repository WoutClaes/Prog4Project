#pragma once
#include "Components/GameComponent.h"
#include <vector>
#include "GameTime.h"

namespace qbert
{
    class QbertComponent;
    class CoilyComponent;
    class SlickSamComponent;
    class UggWrongwayComponent;
    
    class CollisionSystem final : public dae::GameComponent
    {
    public:
        explicit CollisionSystem(dae::GameObject* pOwner)
            : GameComponent(pOwner) {}
        ~CollisionSystem() override = default;

        void Update() override;
        void FixedUpdate() override {}
        void Render() const override {}

        void AddQbert(QbertComponent* qbert) { m_Qberts.push_back(qbert); }
        void AddCoily(CoilyComponent* coily) { m_Coilys.push_back(coily); }
        void AddSlickSam(SlickSamComponent* ss) { m_SlickSams.push_back(ss); }
        void AddUggWrongway(UggWrongwayComponent* uw) { m_UggWrongways.push_back(uw); }

        CollisionSystem(const CollisionSystem&) = delete;
        CollisionSystem& operator=(const CollisionSystem&) = delete;
        CollisionSystem(CollisionSystem&&) = delete;
        CollisionSystem& operator=(CollisionSystem&&) = delete;

    private:
        float m_StartupTimer{ 0.f };
        void CheckQbertVsCoily();
        void CheckQbertVsSlickSam();
        void CheckQbertVsUggWrongway();

        std::vector<QbertComponent*> m_Qberts{};
        std::vector<CoilyComponent*> m_Coilys{};
        std::vector<SlickSamComponent*> m_SlickSams{};
        std::vector<UggWrongwayComponent*> m_UggWrongways{};
    };
}
