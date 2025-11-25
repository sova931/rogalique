#pragma once
#include <vector>
#include <functional>

namespace RogaliqueGame
{
    class IScoreObserver
    {
    public:
        virtual ~IScoreObserver() = default;
        virtual void OnScoreChanged(int newScore) = 0;
        virtual void OnBrickDestroyed(int points) = 0;
    };

    class ScoreSystem
    {
    public:
        void AddObserver(IScoreObserver* observer);
        void RemoveObserver(IScoreObserver* observer);

        void AddScore(int points);
        void BrickDestroyed(int points);
        int GetCurrentScore() const { return currentScore; }
        void Reset();

    private:
        int currentScore = 0;
        std::vector<IScoreObserver*> observers;

        void NotifyScoreChanged();
        void NotifyBrickDestroyed(int points);
    };
}
