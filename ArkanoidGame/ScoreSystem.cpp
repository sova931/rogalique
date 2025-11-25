#include "ScoreSystem.h"

namespace ArkanoidGame
{
    void ScoreSystem::AddObserver(IScoreObserver* observer)
    {
        observers.push_back(observer);
    }

    void ScoreSystem::RemoveObserver(IScoreObserver* observer)
    {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }

    void ScoreSystem::AddScore(int points)
    {
        currentScore += points;
        NotifyScoreChanged();
    }

    void ScoreSystem::BrickDestroyed(int points)
    {
        AddScore(points);
        NotifyBrickDestroyed(points);
    }

    void ScoreSystem::Reset()
    {
        currentScore = 0;
        NotifyScoreChanged();
    }

    void ScoreSystem::NotifyScoreChanged()
    {
        for (auto observer : observers)
        {
            observer->OnScoreChanged(currentScore);
        }
    }

    void ScoreSystem::NotifyBrickDestroyed(int points)
    {
        for (auto observer : observers)
        {
            observer->OnBrickDestroyed(points);
        }
    }
}