#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "Ball.h"
#include "Platform.h"
#include "Text.h" 
#include "Brick.h"
#include "Bonus.h"
#include "ScoreSystem.h"
#include "Memento.h"
#include <vector>
#include <memory>
#include <list>

namespace RogaliqueGame
{
    class GameStatePlayingData
    {
    public:
        GameStatePlayingData();
        ~GameStatePlayingData();

        void Init();
        void Shutdown();
        void HandleWindowEvent(const sf::Event& event);
        void Update(float timeDelta);
        void Draw(sf::RenderWindow& window);

        // Геттеры для игровых объектов
        const Ball& GetBall() const { return ball; }
        Ball& GetBall() { return ball; }
        const Platform& GetPlatform() const { return platform; }
        Platform& GetPlatform() { return platform; }
        const std::vector<Brick>& GetBricks() const { return bricks; }
        std::vector<Brick>& GetBricks() { return bricks; }

        // Новые методы для бонусов
        void SpawnBonus(const sf::Vector2f& position);
        void CheckBonusCollisions();
        void UpdateBonuses(float timeDelta);
        void DrawBonuses(sf::RenderWindow& window);

        // Методы для активных бонусов
        void ActivateBonus(std::unique_ptr<IBonus> bonus);
        void UpdateActiveBonuses(float timeDelta);

        // Система очков
        ScoreSystem& GetScoreSystem() { return scoreSystem; }
        void UpdateScoreDisplay();

        // Система сохранений и жизней
        void SaveGame();
        void LoadGame();
        void SetLives(int newLives);
        int GetLives() const { return lives; }
        void LoseLife();
        void ResetBall();

        // Флаги для бонусов
        bool AreBricksFragile() const { return fragileBricks; }
        void SetFragileBricks(bool fragile) { fragileBricks = fragile; }

        // UI для бонусов и жизней
        void InitUI();
        void UpdateUI();
        void DrawUI(sf::RenderWindow& window);

    private:
        // Ресурсы
        sf::Texture ballTexture;
        sf::Texture brickTexture;
        sf::Font font;

        // Игровые объекты
        Ball ball;
        Platform platform;
        std::vector<Brick> bricks;

        // Система бонусов
        std::list<std::unique_ptr<Bonus>> activeBonuses;
        std::vector<std::pair<std::unique_ptr<IBonus>, float>> activeBonusEffects;

        // Система очков
        ScoreSystem scoreSystem;

        // Система сохранений
        GameOriginator gameOriginator;
        int lives = 3;
        bool isBallActive = false;

        // Флаги бонусов
        bool fragileBricks = false;

        // UI данные
        sf::Text scoreText;
        sf::Text livesText;
        sf::Text winText;
        sf::Text activeBonusesText;
        std::vector<sf::Text> activeBonusNames;

        bool gameWon = false;
        bool gameOver = false;

        // Приватные методы
        void InitBricks();
        void CheckCollisions();
        void HandleBallPlatformCollision();
        void HandleBallBrickCollision();
        void HandleWinCondition();
        void ResetGame();
        void HandleBonusCollection(Bonus* bonus);
        void ProcessBrickDestruction(Brick& brick);
    };

    // Объявления функций для совместимости с существующей архитектурой
    void InitGameStatePlaying(GameStatePlayingData& data);
    void ShutdownGameStatePlaying(GameStatePlayingData& data);
    void HandleGameStatePlayingWindowEvent(GameStatePlayingData& data, const sf::Event& event);
    void UpdateGameStatePlaying(GameStatePlayingData& data, float timeDelta);
    void DrawGameStatePlaying(GameStatePlayingData& data, sf::RenderWindow& window);
}