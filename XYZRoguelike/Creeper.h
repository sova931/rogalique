#pragma once

#include "GameObject.h"
#include "Collidable.h"
#include "Ball.h"
#include <memory>

namespace XYZRoguelike
{
    /// <summary>
    /// Простой ИИ для управления движением криппера
    /// </summary>
    class SimpleAI
    {
    public:
        SimpleAI() = default;

        void SetTarget(const sf::Vector2f& targetPosition);
        void Update(float timeDelta);
        sf::Vector2f GetDirection() const;
        sf::Vector2f GetPosition() const;

        void SetPosition(const sf::Vector2f& position);

    private:
        sf::Vector2f m_position;
        sf::Vector2f m_targetPosition;
        sf::Vector2f m_direction;
    };

    /// <summary>
    /// Класс криппера - вражеского персонажа, который преследует игрока
    /// </summary>
    class Creeper : public GameObject, public Collidable
    {
    public:
        Creeper(const sf::Vector2f& position);

        void Update(float timeDelta) override;
        void Draw(sf::RenderWindow& window) override;
        sf::FloatRect GetRect() const override;
        bool CheckCollision(const std::shared_ptr<Ball>& ball) override;

        void SetTarget(const std::shared_ptr<GameObject>& target);
        void Configure(float speed, int health, int damage);

        sf::Vector2f GetPosition() const override;
        void SetPosition(const sf::Vector2f& position) override;

        bool IsDead() const { return m_health <= 0; }
        void TakeDamage(int damage);

    private:
        std::unique_ptr<SimpleAI> m_ai;
        std::weak_ptr<GameObject> m_target;
        sf::CircleShape m_shape;
        float m_speed;
        int m_health;
        int m_damage;

        void HandleMovement(float timeDelta);
    };
}