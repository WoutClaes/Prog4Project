#include "Subject.h"
#include <algorithm>

namespace dae
{
    void Subject::AddObserver(IObserver* observer)
    {
        if (observer && std::find(m_Observers.begin(), m_Observers.end(), observer) == m_Observers.end())
            m_Observers.push_back(observer);
    }

    void Subject::RemoveObserver(IObserver* observer)
    {
        m_Observers.erase(
            std::remove(m_Observers.begin(), m_Observers.end(), observer),
            m_Observers.end());
    }

    void Subject::Notify(GameObject* pGameObject, Event event)
    {
        auto observers = m_Observers;
        for (auto* observer : observers)
            observer->OnNotify(pGameObject, event);
    }
}
