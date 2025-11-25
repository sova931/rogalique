#include "Brick.h"
#include "Sprite.h"
#include <iostream>

namespace RogaliqueGame
{
    Brick::Brick() : hitPoints(1), maxHitPoints(1)
    {
    }

    void Brick::Init(const sf::Texture& texture, const sf::Vector2f& position, const sf::Color& color, BrickType brickType)
    {
        float width = 64.f;
        float height = 32.f;

        InitSprite(sprite, width, height, texture);
        sprite.setPosition(position);

        type = brickType;
        baseColor = color;

        // Устанавливаем прочность в зависимости от типа
        switch (type)
        {
        case BrickType::Normal:
            hitPoints = maxHitPoints = 1;
            sprite.setColor(color);
            break;
        case BrickType::Strong:
            hitPoints = maxHitPoints = 4;  // Изменили с 3 на 4 удара
            sprite.setColor(color);
            break;
        case BrickType::Unbreakable:
            hitPoints = maxHitPoints = 999; // Практически неразрушим
            sprite.setColor(sf::Color(100, 100, 100)); // Серый цвет
            break;
        case BrickType::Glass:
            hitPoints = maxHitPoints = 1;
            // Сделаем стеклянный блок полупрозрачным
            sf::Color glassColor = color;
            glassColor.a = 128;
            sprite.setColor(glassColor);
            break;
        }
    }

    void Brick::Update(float timeDelta)
    {
        // Блок не требует обновления состояния, если не анимирован
    }

    void Brick::Draw(sf::RenderWindow& window) const
    {
        if (!destroyed)
        {
            window.draw(sprite);
        }
    }

    void Brick::OnBallHit()
    {
        if (type == BrickType::Unbreakable)
            return;

        hitPoints--;

        if (hitPoints <= 0)
        {
            destroyed = true;
        }
        else
        {
            UpdateTexture();
        }
    }

    bool Brick::ShouldBallBounce() const
    {
        // Стеклянные блоки не вызывают отскок
        return type != BrickType::Glass;
    }

    void Brick::UpdateTexture()
    {
        // Изменяем цвет в зависимости от оставшейся прочности
        if (type == BrickType::Strong)
        {
            // Меняем цвет в зависимости от оставшихся HP
            sf::Color newColor;
            switch (hitPoints)
            {
            case 4:
                newColor = sf::Color(50, 150, 255);  // Синий (полная прочность)
                break;
            case 3:
                newColor = sf::Color(100, 200, 100); // Зеленый
                break;
            case 2:
                newColor = sf::Color(255, 200, 50);  // Желтый
                break;
            case 1:
                newColor = sf::Color(255, 100, 50);  // Оранжево-красный
                break;
            default:
                newColor = baseColor;
                break;
            }

            sprite.setColor(newColor);
        }
    }
}