#pragma once
#include "Event.h"

namespace dae
{
    class GameObject;

    class IObserver
    {
    public:
        virtual ~IObserver() = default;
        virtual void OnNotify(GameObject* pGameObject, Event event) = 0;
    };
}
