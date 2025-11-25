// Bonus.h
#pragma once
#include <SFML/Graphics.hpp>
#include "GameSettings.h"
#include "Sprite.h" // Добавляем инклюд для функций работы со спрайтами

namespace ArkanoidGame
{
    class GameStatePlayingData;

    // Базовый интерфейс для бонусов
    class IBonus
    {
    public:
        virtual ~IBonus() = default;
        virtual void Apply(GameStatePlayingData& game) = 0;
        virtual void Remove(GameStatePlayingData& game) = 0;
        virtual sf::Color GetColor() const = 0;
        virtual std::string GetName() const = 0;
        virtual float GetDuration() const = 0;
    };

    // Базовый класс для игровых бонусов
    class Bonus : public sf::Sprite
    {
    public:
        Bonus(std::unique_ptr<IBonus> effect, const sf::Texture& texture);

        void Update(float timeDelta);
        void Draw(sf::RenderWindow& window) const;
        bool IsActive() const { return isActive; }
        void Collect() { isActive = false; }
        std::unique_ptr<IBonus> GetEffect() { return std::move(effect); }
        sf::FloatRect GetGlobalBounds() const;

    private:
        std::unique_ptr<IBonus> effect;
        bool isActive = true;
        float speed = 100.f;
    };

    // Фабрика для создания бонусов
    class BonusFactory
    {
    public:
        static std::unique_ptr<Bonus> CreateRandomBonus(const sf::Vector2f& position);

    private:
        static std::unique_ptr<IBonus> CreateFireBallBonus();
        static std::unique_ptr<IBonus> CreateFragileBricksBonus();
        static std::unique_ptr<IBonus> CreatePlatformSizeBonus(bool enlarge);
        static std::unique_ptr<IBonus> CreatePlatformSpeedBonus(bool faster);
        static std::unique_ptr<IBonus> CreateRandomDirectionBonus();
    };
}