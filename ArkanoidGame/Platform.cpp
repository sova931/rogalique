#include "Platform.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>

namespace ArkanoidGame
{
    Platform::Platform() : speed(500.f)
    {
    }

    void Platform::Init()
    {
        shape.setSize(sf::Vector2f(100.f, 20.f));
        shape.setFillColor(sf::Color::White);
        shape.setOrigin(shape.getSize().x / 2, shape.getSize().y / 2);
        shape.setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEGHT - 50.f);
    }

    void Platform::Update(float timeDelta, const sf::RenderWindow& window)
    {
        HandleInput(timeDelta, window);
    }

    void Platform::Draw(sf::RenderWindow& window) const
    {
        window.draw(shape);
    }

    void Platform::HandleInput(float timeDelta, const sf::RenderWindow& window)
    {
        float movement = 0.f;

        // Управление с клавиатуры
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            movement = -speed * timeDelta;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            movement = speed * timeDelta;
        }

        // Управление мышью
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        float targetX = static_cast<float>(mousePos.x);

        // Ограничиваем целевую позицию в пределах экрана
        float halfWidth = shape.getSize().x / 2;
        if (targetX < halfWidth) targetX = halfWidth;
        if (targetX > SCREEN_WIDTH - halfWidth) targetX = SCREEN_WIDTH - halfWidth;

        // Плавное движение к позиции мыши
        float platformCenterX = shape.getPosition().x;
        float difference = targetX - platformCenterX;

        if (std::abs(difference) > 5.f) // Небольшой порог для предотвращения дрожания
        {
            movement = (difference > 0 ? 1.f : -1.f) * speed * timeDelta;

            // Ограничиваем движение, чтобы не проскочить цель
            if (std::abs(movement) > std::abs(difference))
            {
                movement = difference;
            }
        }

        // Применяем движение с проверкой границ
        float newX = shape.getPosition().x + movement;

        // Ограничиваем движение в пределах экрана
        if (newX - halfWidth >= 0 && newX + halfWidth <= SCREEN_WIDTH)
        {
            shape.setPosition(newX, shape.getPosition().y);
        }
        else
        {
            // Корректируем позицию, если вышли за границы
            if (newX - halfWidth < 0)
                shape.setPosition(halfWidth, shape.getPosition().y);
            else if (newX + halfWidth > SCREEN_WIDTH)
                shape.setPosition(SCREEN_WIDTH - halfWidth, shape.getPosition().y);
        }
    }
}