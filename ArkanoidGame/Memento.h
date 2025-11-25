#pragma once
#include "Ball.h"
#include "Platform.h"
#include "Brick.h"
#include <vector>
#include <memory>

namespace RogaliqueGame
{
    class GameMemento
    {
    public:
        GameMemento(const Ball& ball, const Platform& platform,
            const std::vector<Brick>& bricks, int score, int lives);

        // Методы для восстановления состояния
        Ball GetBallState() const { return ballState; }
        Platform GetPlatformState() const { return platformState; }
        std::vector<Brick> GetBricksState() const { return bricksState; }
        int GetScore() const { return score; }
        int GetLives() const { return lives; }

    private:
        Ball ballState;
        Platform platformState;
        std::vector<Brick> bricksState;
        int score;
        int lives;
    };

    class GameOriginator
    {
    public:
        std::unique_ptr<GameMemento> Save() const;
        void Restore(const GameMemento& memento);

        void SetState(Ball ball, Platform platform,
            std::vector<Brick> bricks, int score, int lives);

    private:
        Ball currentBall;
        Platform currentPlatform;
        std::vector<Brick> currentBricks;
        int currentScore = 0;
        int currentLives = 3;
    };
}