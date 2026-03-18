#include "AddPointsCommand.h"
#include "Components/ScoreComponent.h"

namespace dae
{
    AddPointsCommand::AddPointsCommand(ScoreComponent* pScore, int amount)
        : m_pScore(pScore)
        , m_Amount(amount)
    {}

    void AddPointsCommand::Execute()
    {
        if (m_pScore)
            m_pScore->AddPoints(m_Amount);
    }
}
