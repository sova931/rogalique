#pragma once
#include <SFML/Graphics.hpp>
#include "GameSettings.h"

namespace RogaliqueGame
{
    enum class BrickType
    {
        Normal,     // Обычный - 1 удар
        Strong,     // Прочный - 3 удара  
        Unbreakable,// Неразрушимый
        Glass       // Стеклянный - мяч проходит сквозь
    };

    class Brick
    {
    public:
        Brick();

        void Init(const sf::Texture& texture, const sf::Vector2f& position, const sf::Color& color, BrickType brickType = BrickType::Normal);
        void Update(float timeDelta);
        void Draw(sf::RenderWindow& window) const;

        void OnBallHit();
        bool ShouldBallBounce() const;
        void UpdateTexture();

        // Геттеры
        sf::Vector2f GetPosition() const { return sprite.getPosition(); }
        sf::FloatRect GetGlobalBounds() const { return sprite.getGlobalBounds(); }
        bool IsDestroyed() const { return destroyed; }
        BrickType GetType() const { return type; }
        int GetHitPoints() const { return hitPoints; }

        // Сеттеры
        void SetPosition(const sf::Vector2f& position) { sprite.setPosition(position); }

    private:
        sf::Sprite sprite;
        bool destroyed = false;
        BrickType type = BrickType::Normal;
        int hitPoints = 1;
        int maxHitPoints = 1;
        sf::Color baseColor;
    };
}