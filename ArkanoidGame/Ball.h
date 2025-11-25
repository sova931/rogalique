#pragma once
#include <SFML/Graphics.hpp>
#include "GameSettings.h"

namespace ArkanoidGame
{
    class Ball
    {
    public:
        Ball();

        void Init(const sf::Texture& texture);
        void Update(float timeDelta);
        void Draw(sf::RenderWindow& window) const;

        // Геттеры
        sf::Vector2f GetPosition() const { return sprite.getPosition(); }
        sf::FloatRect GetGlobalBounds() const { return sprite.getGlobalBounds(); }
        sf::Vector2f GetVelocity() const { return velocity; }

        // Сеттеры
        void SetPosition(const sf::Vector2f& position) { sprite.setPosition(position); }
        void SetVelocity(const sf::Vector2f& newVelocity) { velocity = newVelocity; }

        // Вспомогательные методы
        void ReverseVelocityX() { velocity.x = -velocity.x; }
        void ReverseVelocityY() { velocity.y = -velocity.y; }

    private:
        sf::Sprite sprite;
        sf::Vector2f velocity;
        float speed;

        void CheckBoundaryCollision();
    };
}