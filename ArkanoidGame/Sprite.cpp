#include "Sprite.h"
#include "GameSettings.h"
#include <iostream>
#include <cmath>

namespace RogaliqueGame
{
    // ========== ‘”Õ ÷»» ƒÀﬂ SPRITE ==========

    void InitSprite(sf::Sprite& sprite, float desiredWidth, float desiredHeight, const sf::Texture& texture)
    {
        std::cout << "Initializing sprite with texture size: "
            << texture.getSize().x << " x " << texture.getSize().y << std::endl;

        sprite.setTexture(texture);
        SetSpriteRelativeOrigin(sprite, 0.5f, 0.5f);
        SetSpriteSize(sprite, desiredWidth, desiredHeight);

        std::cout << "Sprite initialized. Final bounds: "
            << sprite.getGlobalBounds().width << " x " << sprite.getGlobalBounds().height << std::endl;
    }

    void DrawSprite(const sf::Sprite& sprite, sf::RenderWindow& window)
    {
        window.draw(sprite);
    }

    sf::Vector2f GetRandomSpritePositionInRectangle(const sf::Sprite& sprite, const sf::FloatRect& rect)
    {
        sf::Vector2f result;
        const auto spriteWidth = sprite.getGlobalBounds().width;
        const auto spriteHeight = sprite.getGlobalBounds().height;
        result.x = rand() / (float)RAND_MAX * (rect.width - 2 * spriteWidth) + rect.left + spriteWidth;
        result.y = rand() / (float)RAND_MAX * (rect.height - 2 * spriteHeight) + rect.top + spriteHeight;
        return result;
    }

    void SetSpriteSize(sf::Sprite& sprite, float desiredWidth, float desiredHeight)
    {
        sf::FloatRect spriteRect = sprite.getLocalBounds();
        if (spriteRect.width == 0 || spriteRect.height == 0)
        {
            std::cout << "Warning: Sprite has zero bounds!" << std::endl;
            return;
        }

        sf::Vector2f scale = { desiredWidth / spriteRect.width, desiredHeight / spriteRect.height };
        sprite.setScale(scale);

        std::cout << "Sprite scaled to: " << desiredWidth << " x " << desiredHeight
            << " (scale: " << scale.x << ", " << scale.y << ")" << std::endl;
    }

    void SetSpriteRelativeOrigin(sf::Sprite& sprite, float originX, float originY)
    {
        sf::FloatRect spriteRect = sprite.getLocalBounds();
        sprite.setOrigin(originX * spriteRect.width, originY * spriteRect.height);
    }

    void SetSpriteRandomPosition(sf::Sprite& sprite, const sf::FloatRect& rect, const std::list<sf::Sprite>& collection)
    {
        do {
            const auto newPosition = GetRandomSpritePositionInRectangle(sprite, rect);
            sprite.setPosition(newPosition);
        } while (FullCheckCollisions(collection.begin(), collection.end(), sprite));
    }

    bool CheckSpriteIntersection(const sf::Sprite& sprite1, const sf::Sprite& sprite2) {
        const auto distance = sprite1.getPosition() - sprite2.getPosition();
        if (sqrtf(distance.x * distance.x + distance.y * distance.y) * 2 < sprite1.getGlobalBounds().width + sprite2.getGlobalBounds().width) {
            return true;
        }
        return false;
    }

    sf::Vector2f GetVectorBetweenSprites(const sf::Sprite& spriteFrom, const sf::Sprite& spriteTo) {
        const auto result = spriteTo.getPosition() - spriteFrom.getPosition();
        return result;
    }

    float GetManhattanDistanceBetweenSprites(const sf::Sprite& spriteFrom, const sf::Sprite& spriteTo) {
        const auto result = spriteTo.getPosition() - spriteFrom.getPosition();
        return std::fabs(result.x) + std::fabs(result.y);
    }

    // ========== ‘”Õ ÷»» ƒÀﬂ TEXT ==========

    void SetTextRelativeOrigin(sf::Text& text, float originX, float originY)
    {
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(originX * textRect.width, originY * textRect.height);
    }

    void SetTextSize(sf::Text& text, float desiredWidth, float desiredHeight)
    {
        sf::FloatRect textRect = text.getLocalBounds();
        if (textRect.width == 0 || textRect.height == 0)
        {
            return;
        }

        sf::Vector2f scale = { desiredWidth / textRect.width, desiredHeight / textRect.height };
        text.setScale(scale);
    }
}