# Боевая система XYZRoguelike

## Обзор
Боевая система основана на компонентной архитектуре, что позволяет легко расширять функционал.

## Основные компоненты

### AttackComponent
Отвечает за нанесение урона. Содержит:
- Урон
- Скорость атаки
- Радиус атаки
- Таймер перезарядки

### StatsComponent
Хранит статистики персонажа:
- Здоровье
- Броня
- Скорость
- Урон

### AI Component
Управляет поведением врагов:
- Состояния (Патрулирование, Преследование, Атака)
- Детектирование игрока
- Принятие решений

## Взаимодействие компонентов

1. **Detect** - AI обнаруживает игрока
2. **Move** - Персонаж двигается к цели
3. **Attack** - AttackComponent проверяет возможность атаки
4. **Damage** - Наносится урон через StatsComponent цели
5. **Death** - Проверка здоровья и уничтожение объекта

## Наследование
GameObject
├── Player
├── Creeper (новый класс)
│ ├── AI (поведение)
│ ├── AttackComponent
│ └── StatsComponent
└── OtherEnemies

## Расширение системы

Для добавления нового типа врага:
1. Наследовать от GameObject
2. Добавить необходимые компоненты
3. Реализовать интерфейс Collidable
4. Настроить параметры в CreeperSpawner

## Пример конфигурации
```cpp
// Настройка врага через фабрику
EnemyFactory::Create("Creeper", {
    {"health", 100},
    {"damage", 15},
    {"speed", 120},
    {"behavior", "Aggressive"}
});


## 6. Работа с Git

**Рекомендуемая структура веток:**
main
├── feature/clang-format # Форматирование кода
├── feature/creeper-class # Класс криппера
├── feature/spawner-system # Система спауна
├── feature/combat-docs # Документация
└── develop # Ветка для тестирования

**Команды для работы:**
```bash
# Создание веток
git checkout -b feature/creeper-class
git checkout -b feature/spawner-system

# Форматирование кода
./format_all.bat

# Коммиты с описательными сообщениями
git commit -m "feat: add Creeper class with AI behavior"
git commit -m "feat: implement dynamic spawner system"
git commit -m "docs: add combat system documentation"

# Слияние в develop
git checkout develop
git merge --no-ff feature/creeper-class
