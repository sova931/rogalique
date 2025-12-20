#pragma once

#include "GameObject.h"
#include "Creeper.h"
#include <vector>
#include <memory>
#include <functional>

namespace XYZRoguelike
{
    /// <summary>
    /// Класс для управления спауном крипперов
    /// Поддерживает динамическую настройку параметров спауна
    /// </summary>
    class CreeperSpawner
    {
    public:
        /// <summary>
        /// Конфигурация спаунера для гибкой настройки
        /// </summary>
        struct SpawnConfig
        {
            int minCreepers = 1;           // Минимальное количество
            int maxCreepers = 5;           // Максимальное количество
            float minSpeed = 80.0f;        // Минимальная скорость
            float maxSpeed = 150.0f;       // Максимальная скорость
            int minHealth = 50;            // Минимальное здоровье
            int maxHealth = 150;           // Максимальное здоровье
            float spawnRadius = 100.0f;    // Радиус спауна от игрока

            // Функция для проверки валидности позиции спауна
            std::function<bool(const sf::Vector2f&)> positionValidator = nullptr;
        };

        CreeperSpawner() = default;

        /// <summary>
        /// Инициализация спаунера
        /// </summary>
        void Initialize(const std::shared_ptr<GameObject>& player,
            const SpawnConfig& config = SpawnConfig());

        /// <summary>
        /// Спаун крипперов в случайных позициях
        /// </summary>
        std::vector<std::shared_ptr<Creeper>> SpawnCreepers();

        /// <summary>
        /// Обновление состояния спаунера
        /// </summary>
        void Update(float timeDelta);

        /// <summary>
        /// Получение всех активных крипперов
        /// </summary>
        const std::vector<std::shared_ptr<Creeper>>& GetActiveCreepers() const;

        /// <summary>
        /// Очистка всех крипперов (при смене уровня)
        /// </summary>
        void Clear();

    private:
        std::vector<std::shared_ptr<Creeper>> m_activeCreepers;
        std::shared_ptr<GameObject> m_player;
        SpawnConfig m_config;
        bool m_initialized = false;

        /// <summary>
        /// Генерация случайной позиции для спауна
        /// </summary>
        sf::Vector2f GenerateSpawnPosition() const;
    };
}