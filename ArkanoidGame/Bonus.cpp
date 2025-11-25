#include "Bonus.h"
#include "GameStatePlaying.h"
#include <random>

namespace RogaliqueGame
{
    Bonus::Bonus(std::unique_ptr<IBonus> effect, const sf::Texture& texture)
        : effect(std::move(effect))
    {
        setTexture(texture);
        SetSpriteSize(*this, 30.f, 30.f);
        SetSpriteRelativeOrigin(*this, 0.5f, 0.5f);
    }

    void Bonus::Update(float timeDelta)
    {
        if (isActive)
        {
            move(0, speed * timeDelta);

            // Проверка выхода за нижнюю границу
            if (getPosition().y > SCREEN_HEGHT + 50.f)
            {
                isActive = false;
            }
        }
    }

    void Bonus::Draw(sf::RenderWindow& window) const
    {
        if (isActive)
        {
            window.draw(*this);
        }
    }

    sf::FloatRect Bonus::GetGlobalBounds() const
    {
        return getGlobalBounds();
    }

    // Конкретные реализации бонусов
    class FireBallBonus : public IBonus
    {
    public:
        void Apply(GameStatePlayingData& game) override
        {
            auto& ball = game.GetBall();
            // Сохраняем оригинальную скорость
            originalSpeed = GetVectorLength(ball.GetVelocity());

            // Увеличиваем скорость на 50%
            sf::Vector2f newVelocity = ball.GetVelocity() * 1.5f;
            ball.SetVelocity(newVelocity);

            // Меняем цвет мяча
            // Это потребует добавления метода SetColor в класс Ball
        }

        void Remove(GameStatePlayingData& game) override
        {
            auto& ball = game.GetBall();
            // Восстанавливаем оригинальную скорость
            sf::Vector2f currentVel = ball.GetVelocity();
            float currentLength = GetVectorLength(currentVel);
            sf::Vector2f normalized = currentVel / currentLength;
            ball.SetVelocity(normalized * originalSpeed);

            // Восстанавливаем цвет
        }

        sf::Color GetColor() const override { return sf::Color::Red; }
        std::string GetName() const override { return "Fire Ball"; }
        float GetDuration() const override { return 8.0f; }

    private:
        float originalSpeed = 0.f;
    };

    class FragileBricksBonus : public IBonus
    {
    public:
        void Apply(GameStatePlayingData& game) override
        {
            // Этот бонус будет обрабатываться в GameStatePlayingData
            // Устанавливаем флаг хрупких блоков
        }

        void Remove(GameStatePlayingData& game) override
        {
            // Снимаем флаг хрупких блоков
        }

        sf::Color GetColor() const override { return sf::Color::Cyan; }
        std::string GetName() const override { return "Fragile Bricks"; }
        float GetDuration() const override { return 10.0f; }
    };

    class PlatformSizeBonus : public IBonus
    {
    public:
        PlatformSizeBonus(bool enlarge) : shouldEnlarge(enlarge) {}

        void Apply(GameStatePlayingData& game) override
        {
            auto& platform = game.GetPlatform();
            originalSize = platform.GetWidth();

            float newSize = shouldEnlarge ? originalSize * 1.5f : originalSize * 0.7f;
            // Здесь нужно добавить метод SetSize в Platform
        }

        void Remove(GameStatePlayingData& game) override
        {
            auto& platform = game.GetPlatform();
            // Восстанавливаем оригинальный размер
        }

        sf::Color GetColor() const override
        {
            return shouldEnlarge ? sf::Color::Green : sf::Color::Yellow;
        }

        std::string GetName() const override
        {
            return shouldEnlarge ? "Big Platform" : "Small Platform";
        }

        float GetDuration() const override { return 12.0f; }

    private:
        bool shouldEnlarge;
        float originalSize = 0.f;
    };

    // Реализация фабрики
    std::unique_ptr<Bonus> BonusFactory::CreateRandomBonus(const sf::Vector2f& position)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, 4);

        std::unique_ptr<IBonus> bonus;
        int choice = dis(gen);

        switch (choice)
        {
        case 0:
            bonus = CreateFireBallBonus();
            break;
        case 1:
            bonus = CreateFragileBricksBonus();
            break;
        case 2:
            bonus = CreatePlatformSizeBonus(true);
            break;
        case 3:
            bonus = CreatePlatformSizeBonus(false);
            break;
        case 4:
            bonus = CreateRandomDirectionBonus();
            break;
        default:
            bonus = CreateFireBallBonus();
            break;
        }

        // Создаем текстуру для бонуса (в реальном коде нужно загружать из файлов)
        static sf::Texture bonusTexture;
        static bool textureLoaded = false;

        if (!textureLoaded)
        {
            // Создаем простую текстуру для бонуса
            sf::Image image;
            image.create(32, 32, sf::Color::Transparent);

            // Рисуем круг
            for (unsigned int x = 0; x < 32; ++x)
            {
                for (unsigned int y = 0; y < 32; ++y)
                {
                    float dx = static_cast<float>(x) - 16.f;
                    float dy = static_cast<float>(y) - 16.f;
                    float distance = std::sqrt(dx * dx + dy * dy);

                    if (distance <= 14.f)
                    {
                        image.setPixel(x, y, bonus->GetColor());
                    }
                }
            }

            bonusTexture.loadFromImage(image);
            textureLoaded = true;
        }

        auto bonusObj = std::make_unique<Bonus>(std::move(bonus), bonusTexture);
        bonusObj->setPosition(position);
        return bonusObj;
    }

    std::unique_ptr<IBonus> BonusFactory::CreateFireBallBonus()
    {
        return std::make_unique<FireBallBonus>();
    }

    std::unique_ptr<IBonus> BonusFactory::CreateFragileBricksBonus()
    {
        return std::make_unique<FragileBricksBonus>();
    }

    std::unique_ptr<IBonus> BonusFactory::CreatePlatformSizeBonus(bool enlarge)
    {
        return std::make_unique<PlatformSizeBonus>(enlarge);
    }

    std::unique_ptr<IBonus> BonusFactory::CreatePlatformSpeedBonus(bool faster)
    {
        // Аналогично PlatformSizeBonus
        return std::make_unique<PlatformSizeBonus>(faster);
    }

    std::unique_ptr<IBonus> BonusFactory::CreateRandomDirectionBonus()
    {
        // Аналогично другим бонусам
        return std::make_unique<PlatformSizeBonus>(true);
    }
}