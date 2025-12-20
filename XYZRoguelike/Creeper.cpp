#include "Creeper.h"
#include "GameSettings.h"
#include <cmath>

namespace XYZRoguelike
{
    // Реализация SimpleAI
    void SimpleAI::SetTarget(const sf::Vector2f& targetPosition)
    {
        m_targetPosition = targetPosition;
    }

    void SimpleAI::Update(float timeDelta)
    {
        if (m_position != m_targetPosition)
        {
            m_direction = m_targetPosition - m_position;
            float length = std::sqrt(m_direction.x * m_direction.x + m_direction.y * m_direction.y);
            if (length > 0.0f)
            {
                m_direction /= length;
            }
        }
    }

    sf::Vector2f SimpleAI::GetDirection() const
    {
        return m_direction;
    }

    sf::Vector2f SimpleAI::GetPosition() const
    {
        return m_position;
    }

    void SimpleAI::SetPosition(const sf::Vector2f& position)
    {
        m_position = position;
    }

    // Реализация Creeper
    Creeper::Creeper(const sf::Vector2f& position)
        : m_speed(100.0f), m_health(100), m_damage(10)
    {
        m_ai = std::make_unique<SimpleAI>();
        m_shape.setRadius(20.0f);
        m_shape.setFillColor(sf::Color::Green);
        m_shape.setOutlineColor(sf::Color::White);
        m_shape.setOutlineThickness(2.0f);
        m_shape.setPosition(position);
        m_ai->SetPosition(position);
    }

    void Creeper::Update(float timeDelta)
    {
        if (auto target = m_target.lock())
        {
            m_ai->SetTarget(target->GetPosition());
            m_ai->Update(timeDelta);
            HandleMovement(timeDelta);
        }
    }

    void Creeper::Draw(sf::RenderWindow& window)
    {
        window.draw(m_shape);
    }

    sf::FloatRect Creeper::GetRect() const
    {
        return m_shape.getGlobalBounds();
    }

    bool Creeper::CheckCollision(const std::shared_ptr<Ball>& ball)
    {
        if (!ball) return false;

        sf::FloatRect creeperBounds = GetRect();
        sf::FloatRect ballBounds = ball->GetRect();

        return creeperBounds.intersects(ballBounds);
    }

    void Creeper::SetTarget(const std::shared_ptr<GameObject>& target)
    {
        m_target = target;
        if (target)
        {
            m_ai->SetTarget(target->GetPosition());
        }
    }

    void Creeper::Configure(float speed, int health, int damage)
    {
        m_speed = speed;
        m_health = health;
        m_damage = damage;
    }

    sf::Vector2f Creeper::GetPosition() const
    {
        return m_shape.getPosition();
    }

    void Creeper::SetPosition(const sf::Vector2f& position)
    {
        m_shape.setPosition(position);
        m_ai->SetPosition(position);
    }

    void Creeper::TakeDamage(int damage)
    {
        m_health -= damage;
        if (m_health < 0) m_health = 0;

        // Эффект при получении урона
        m_shape.setFillColor(sf::Color::Red);
        // Здесь можно добавить таймер для сброса цвета
    }

    void Creeper::HandleMovement(float timeDelta)
    {
        sf::Vector2f direction = m_ai->GetDirection();
        sf::Vector2f movement = direction * m_speed * timeDelta;

        // Проверяем границы экрана
        sf::Vector2f newPosition = GetPosition() + movement;

        if (newPosition.x < 0 || newPosition.x > SETTINGS.SCREEN_WIDTH - m_shape.getRadius() * 2)
        {
            direction.x *= -1;
        }
        if (newPosition.y < 0 || newPosition.y > SETTINGS.SCREEN_HEIGHT - m_shape.getRadius() * 2)
        {
            direction.y *= -1;
        }

        movement = direction * m_speed * timeDelta;
        m_shape.move(movement);
        m_ai->SetPosition(m_shape.getPosition());
    }
}