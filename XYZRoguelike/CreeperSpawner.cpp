#include "CreeperSpawner.h"
#include "randomizer.h"
#include "GameSettings.h"
#include <cmath>

namespace XYZRoguelike
{
    void CreeperSpawner::Initialize(const std::shared_ptr<GameObject>& player,
        const SpawnConfig& config)
    {
        m_player = player;
        m_config = config;
        m_initialized = true;
        m_activeCreepers.clear();
    }

    std::vector<std::shared_ptr<Creeper>> CreeperSpawner::SpawnCreepers()
    {
        if (!m_initialized || !m_player)
        {
            return {};
        }

        int creeperCount = random(m_config.minCreepers, m_config.maxCreepers);
        std::vector<std::shared_ptr<Creeper>> newCreepers;

        for (int i = 0; i < creeperCount; ++i)
        {
            sf::Vector2f spawnPos = GenerateSpawnPosition();

            // Проверяем валидность позиции
            if (m_config.positionValidator && !m_config.positionValidator(spawnPos))
            {
                // Пробуем другую позицию
                spawnPos = GenerateSpawnPosition();
            }

            auto creeper = std::make_shared<Creeper>(spawnPos);

            // Динамическая настройка параметров
            float speed = random(m_config.minSpeed, m_config.maxSpeed);
            int health = random(m_config.minHealth, m_config.maxHealth);
            int damage = 10; // Базовый урон

            creeper->Configure(speed, health, damage);
            creeper->SetTarget(m_player);

            newCreepers.push_back(creeper);
            m_activeCreepers.push_back(creeper);
        }

        return newCreepers;
    }

    void CreeperSpawner::Update(float timeDelta)
    {
        // Удаление уничтоженных крипперов
        auto it = std::remove_if(m_activeCreepers.begin(), m_activeCreepers.end(),
            [](const auto& creeper) {
                // Здесь должна быть проверка на уничтожение
                // Например: return creeper->IsDead();
                return false; // Заглушка
            });

        m_activeCreepers.erase(it, m_activeCreepers.end());

        // Обновление активных крипперов
        for (auto& creeper : m_activeCreepers)
        {
            if (creeper)
            {
                creeper->Update(timeDelta);
            }
        }
    }

    const std::vector<std::shared_ptr<Creeper>>& CreeperSpawner::GetActiveCreepers() const
    {
        return m_activeCreepers;
    }

    void CreeperSpawner::Clear()
    {
        m_activeCreepers.clear();
    }

    sf::Vector2f CreeperSpawner::GenerateSpawnPosition() const
    {
        if (!m_player)
        {
            return { SETTINGS.SCREEN_WIDTH / 2.0f, SETTINGS.SCREEN_HEIGHT / 2.0f };
        }

        // Генерация позиции вне радиуса игрока
        sf::Vector2f playerPos = m_player->GetPosition();
        sf::Vector2f spawnPos;

        float angle = random(0.0f, 2.0f * 3.14159f);
        float distance = random(m_config.spawnRadius, m_config.spawnRadius * 2.0f);

        spawnPos.x = playerPos.x + std::cos(angle) * distance;
        spawnPos.y = playerPos.y + std::sin(angle) * distance;

        // Ограничиваем позицию в пределах экрана
        spawnPos.x = std::clamp(spawnPos.x, 0.0f, static_cast<float>(SETTINGS.SCREEN_WIDTH));
        spawnPos.y = std::clamp(spawnPos.y, 0.0f, static_cast<float>(SETTINGS.SCREEN_HEIGHT));

        return spawnPos;
    }
}