#pragma once
#include <SFML/Graphics.hpp>
#include "GameSettings.h"

namespace RogaliqueGame
{
    class Platform
    {
    public:
        Platform();

        void Init();
        void Update(float timeDelta, const sf::RenderWindow& window);
        void Draw(sf::RenderWindow& window) const;

        // Геттеры
        sf::Vector2f GetPosition() const { return shape.getPosition(); }
        sf::FloatRect GetGlobalBounds() const { return shape.getGlobalBounds(); }
        float GetWidth() const { return shape.getSize().x; }
        float GetHeight() const { return shape.getSize().y; }

        // Сеттеры
        void SetPosition(const sf::Vector2f& position) { shape.setPosition(position); }

    private:
        sf::RectangleShape shape;
        float speed;

        void HandleInput(float timeDelta, const sf::RenderWindow& window);
    };
}