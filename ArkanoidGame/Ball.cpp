#include "Ball.h"
#include "Sprite.h"
#include <cassert>
#include <iostream>
#include <cmath>

namespace RogaliqueGame
{
    Ball::Ball() : speed(400.f), velocity(0.f, 0.f)
    {
    }

    void Ball::Init(const sf::Texture& texture)
    {
        std::cout << "Initializing ball..." << std::endl;

        // Проверяем текстуру
        if (texture.getSize().x == 0 || texture.getSize().y == 0)
        {
            std::cout << "Warning: Ball texture is empty!" << std::endl;
        }

        InitSprite(sprite, 40.f,40.f, texture);
        sprite.setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEGHT - 150.f);

        // Устанавливаем начальную скорость
        velocity = sf::Vector2f(0.f, 0.f);

        std::cout << "Ball initialized at position: "
            << sprite.getPosition().x << ", " << sprite.getPosition().y << std::endl;
        std::cout << "Ball bounds: " << sprite.getGlobalBounds().width
            << " x " << sprite.getGlobalBounds().height << std::endl;
    }

    void Ball::Update(float timeDelta)
    {
        // Обновляем позицию только если мяч движется
        if (velocity.x != 0 || velocity.y != 0)
        {
            sf::Vector2f oldPosition = sprite.getPosition();
            sprite.move(velocity * timeDelta);
            CheckBoundaryCollision();

            // Отладочный вывод (можно закомментировать)
            // std::cout << "Ball moved from (" << oldPosition.x << ", " << oldPosition.y 
            //          << ") to (" << sprite.getPosition().x << ", " << sprite.getPosition().y 
            //          << ") velocity: (" << velocity.x << ", " << velocity.y << ")" << std::endl;
        }
    }

    void Ball::Draw(sf::RenderWindow& window) const
    {
        window.draw(sprite);
    }

    void Ball::CheckBoundaryCollision()
    {
        sf::FloatRect bounds = sprite.getGlobalBounds();
        sf::Vector2f position = sprite.getPosition();

        // Столкновение с левой границей
        if (position.x - bounds.width / 2 <= 0)
        {
            sprite.setPosition(bounds.width / 2, position.y);
            ReverseVelocityX();
        }
        // Столкновение с правой границей
        else if (position.x + bounds.width / 2 >= SCREEN_WIDTH)
        {
            sprite.setPosition(SCREEN_WIDTH - bounds.width / 2, position.y);
            ReverseVelocityX();
        }

        // Столкновение с верхней границей
        if (position.y - bounds.height / 2 <= 0)
        {
            sprite.setPosition(position.x, bounds.height / 2);
            ReverseVelocityY();
        }

        // Отладочный вывод при столкновении
        // if (position.x - bounds.width / 2 <= 0 || position.x + bounds.width / 2 >= SCREEN_WIDTH ||
        //     position.y - bounds.height / 2 <= 0)
        // {
        //     std::cout << "Ball collision detected!" << std::endl;
        // }
    }
}