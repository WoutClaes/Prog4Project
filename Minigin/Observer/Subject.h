#pragma once
#include <vector>
#include "IObserver.h"
#include "Event.h"

namespace dae
{
    class GameObject;

    class Subject
    {
    public:
        virtual ~Subject() = default;

        void AddObserver(IObserver* observer);
        void RemoveObserver(IObserver* observer);

    protected:
        void Notify(GameObject* pGameObject, Event event);

    private:
        std::vector<IObserver*> m_Observers{};
    };
}
