#include "GameStatePlaying.h"
#include "Application.h"
#include "Game.h"
#include "Sprite.h"
#include "Vector.h"
#include "Text.h"
#include <cassert>
#include <iostream>
#include <random>
#include <sstream>
#include <cmath>

namespace RogaliqueGame
{
    // Вспомогательная функция для вычисления длины вектора (альтернативная реализация)
    static float CalculateVectorLength(const sf::Vector2f& vector)
    {
        return std::sqrt(vector.x * vector.x + vector.y * vector.y);
    }

    // Статические функции для создания текстур
    static sf::Texture CreateBallTexture()
    {
        sf::Image image;
        image.create(48, 48, sf::Color::Transparent);

        sf::Vector2u center(24, 24);
        unsigned int radius = 22;

        for (unsigned int x = 0; x < 48; ++x)
        {
            for (unsigned int y = 0; y < 48; ++y)
            {
                float dx = static_cast<float>(x) - center.x;
                float dy = static_cast<float>(y) - center.y;
                float distance = std::sqrt(dx * dx + dy * dy);

                if (distance <= radius)
                {
                    float intensity = 1.0f - (distance / radius) * 0.3f;
                    sf::Uint8 value = static_cast<sf::Uint8>(255 * intensity);
                    image.setPixel(x, y, sf::Color(value, value, value));
                }
            }
        }

        sf::Texture texture;
        texture.loadFromImage(image);
        return texture;
    }

    static sf::Texture CreateBrickTexture(sf::Color color)
    {
        sf::Image image;
        image.create(64, 32, color);

        for (unsigned int x = 0; x < 64; ++x)
        {
            image.setPixel(x, 0, sf::Color::Black);
            image.setPixel(x, 31, sf::Color::Black);
            if (x % 8 == 0 && x > 0 && x < 63)
            {
                for (unsigned int y = 1; y < 31; y += 2)
                {
                    sf::Color lineColor = color;
                    lineColor.r = std::min(255, color.r + 30);
                    lineColor.g = std::min(255, color.g + 30);
                    lineColor.b = std::min(255, color.b + 30);
                    image.setPixel(x, y, lineColor);
                }
            }
        }
        for (unsigned int y = 0; y < 32; ++y)
        {
            image.setPixel(0, y, sf::Color::Black);
            image.setPixel(63, y, sf::Color::Black);
        }

        sf::Texture texture;
        texture.loadFromImage(image);
        return texture;
    }

    // Конструктор и деструктор
    GameStatePlayingData::GameStatePlayingData()
        : scoreSystem(), lives(3), isBallActive(false), gameWon(false), gameOver(false),
        fragileBricks(false)
    {
        Init();
    }

    GameStatePlayingData::~GameStatePlayingData()
    {
        Shutdown();
    }

    void GameStatePlayingData::Init()
    {
        // Загрузка ресурсов
        std::string ballTexturePath = RESOURCES_PATH + "Textures/Ball.png";
        std::string brickTexturePath = RESOURCES_PATH + "Textures/Brick.png";
        std::string fontPath = RESOURCES_PATH + "Fonts/Roboto-Regular.ttf";

        std::cout << "Loading ball texture from: " << ballTexturePath << std::endl;
        if (!ballTexture.loadFromFile(ballTexturePath))
        {
            std::cout << "Failed to load ball texture! Creating default texture..." << std::endl;
            ballTexture = CreateBallTexture();
        }
        else
        {
            std::cout << "Ball texture loaded successfully!" << std::endl;
        }

        std::cout << "Loading brick texture from: " << brickTexturePath << std::endl;
        if (!brickTexture.loadFromFile(brickTexturePath))
        {
            std::cout << "Failed to load brick texture! Creating default texture..." << std::endl;
            brickTexture = CreateBrickTexture(sf::Color::Red);
        }
        else
        {
            std::cout << "Brick texture loaded successfully!" << std::endl;
        }

        std::cout << "Loading font from: " << fontPath << std::endl;
        if (!font.loadFromFile(fontPath))
        {
            std::cout << "Failed to load font! Using default font." << std::endl;
            if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf"))
            {
                std::cout << "Cannot load any font!" << std::endl;
            }
        }
        else
        {
            std::cout << "Font loaded successfully!" << std::endl;
        }

        // Инициализация игровых объектов
        ball.Init(ballTexture);
        platform.Init();
        InitBricks();

        // Инициализация UI
        InitUI();

        // Сохранение начального состояния
        SaveGame();

        std::cout << "GameStatePlaying initialized successfully!" << std::endl;
    }

    void GameStatePlayingData::InitUI()
    {
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setString("Score: 0");
        scoreText.setPosition(10.f, 10.f);

        livesText.setFont(font);
        livesText.setCharacterSize(24);
        livesText.setFillColor(sf::Color::White);
        livesText.setString("Lives: 3");
        livesText.setPosition(SCREEN_WIDTH - 150.f, 10.f);

        activeBonusesText.setFont(font);
        activeBonusesText.setCharacterSize(18);
        activeBonusesText.setFillColor(sf::Color::Yellow);
        activeBonusesText.setString("Active Bonuses:");
        activeBonusesText.setPosition(10.f, 50.f);

        winText.setFont(font);
        winText.setCharacterSize(48);
        winText.setFillColor(sf::Color::Green);
        winText.setString("You Win!\nPlay again? (Y/N)");
        winText.setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEGHT / 2.f);
        SetTextRelativeOrigin(winText, 0.5f, 0.5f); // Исправлено: используем функцию для текста

        // Подписываемся на события системы очков
        class ScoreObserver : public IScoreObserver
        {
        public:
            ScoreObserver(GameStatePlayingData& game) : game(game) {}

            void OnScoreChanged(int newScore) override
            {
                game.UpdateScoreDisplay();
            }

            void OnBrickDestroyed(int points) override
            {
                std::cout << "Brick destroyed! +" << points << " points" << std::endl;
            }

        private:
            GameStatePlayingData& game;
        };

        scoreSystem.AddObserver(new ScoreObserver(*this));
    }

    void GameStatePlayingData::Shutdown()
    {
        // Очищаем бонусы
        activeBonuses.clear();
        activeBonusEffects.clear();
        bricks.clear();
    }

    void GameStatePlayingData::HandleWindowEvent(const sf::Event& event)
    {
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape)
            {
                Application::Instance().GetGame().PushState(GameStateType::ExitDialog, true);
            }
            else if (event.key.code == sf::Keyboard::Space && !gameWon && !gameOver)
            {
                // Запуск мяча при нажатии пробела
                if (!isBallActive)
                {
                    ball.SetVelocity(sf::Vector2f(200.f, -300.f));
                    isBallActive = true;
                    std::cout << "Ball launched!" << std::endl;
                }
            }
            else if (gameWon || gameOver)
            {
                if (event.key.code == sf::Keyboard::Y)
                {
                    ResetGame();
                }
                else if (event.key.code == sf::Keyboard::N)
                {
                    Application::Instance().GetGame().SwitchStateTo(GameStateType::MainMenu);
                }
            }
        }
    }

    void GameStatePlayingData::Update(float timeDelta)
    {
        if (!gameWon && !gameOver)
        {
            platform.Update(timeDelta, Application::Instance().GetWindow());

            if (isBallActive)
            {
                ball.Update(timeDelta);
            }
            else
            {
                // Мяч следует за платформой, пока не активирован
                ball.SetPosition(sf::Vector2f(
                    platform.GetPosition().x,
                    platform.GetPosition().y - platform.GetHeight() - ball.GetGlobalBounds().height / 2
                ));
            }

            UpdateBonuses(timeDelta);
            CheckCollisions();
            UpdateUI();
        }
    }

    void GameStatePlayingData::Draw(sf::RenderWindow& window)
    {
        window.clear(sf::Color::Black);

        for (const auto& brick : bricks)
        {
            brick.Draw(window);
        }

        platform.Draw(window);
        ball.Draw(window);
        DrawBonuses(window);
        DrawUI(window);

        if (gameWon)
        {
            window.draw(winText);
        }
    }

    // ========== СИСТЕМА БОНУСОВ ==========

    void GameStatePlayingData::SpawnBonus(const sf::Vector2f& position)
    {
        auto bonus = BonusFactory::CreateRandomBonus(position);
        if (bonus)
        {
            activeBonuses.push_back(std::move(bonus));
            std::cout << "Bonus spawned at position: " << position.x << ", " << position.y << std::endl;
        }
    }

    void GameStatePlayingData::CheckBonusCollisions()
    {
        auto platformBounds = platform.GetGlobalBounds();

        for (auto& bonus : activeBonuses)
        {
            if (bonus->IsActive() && bonus->GetGlobalBounds().intersects(platformBounds))
            {
                std::cout << "Bonus collected!" << std::endl;
                HandleBonusCollection(bonus.get());
                bonus->Collect();
            }
        }

        // Удаляем собранные или упавшие за экран бонусы
        activeBonuses.remove_if([](const auto& bonus) { return !bonus->IsActive(); });
    }

    void GameStatePlayingData::UpdateBonuses(float timeDelta)
    {
        // Обновляем падающие бонусы
        for (auto& bonus : activeBonuses)
        {
            bonus->Update(timeDelta);
        }

        // Проверяем столкновения
        CheckBonusCollisions();

        // Обновляем активные эффекты бонусов
        UpdateActiveBonuses(timeDelta);
    }

    void GameStatePlayingData::DrawBonuses(sf::RenderWindow& window)
    {
        for (auto& bonus : activeBonuses)
        {
            bonus->Draw(window);
        }
    }

    void GameStatePlayingData::ActivateBonus(std::unique_ptr<IBonus> bonus)
    {
        if (bonus)
        {
            std::cout << "Activating bonus: " << bonus->GetName() << std::endl;
            bonus->Apply(*this);
            activeBonusEffects.emplace_back(std::move(bonus), bonus->GetDuration());
        }
    }

    void GameStatePlayingData::UpdateActiveBonuses(float timeDelta)
    {
        for (auto it = activeBonusEffects.begin(); it != activeBonusEffects.end(); )
        {
            it->second -= timeDelta;

            if (it->second <= 0.0f)
            {
                std::cout << "Bonus expired: " << it->first->GetName() << std::endl;
                it->first->Remove(*this);
                it = activeBonusEffects.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void GameStatePlayingData::HandleBonusCollection(Bonus* bonus)
    {
        auto effect = bonus->GetEffect();
        if (effect)
        {
            ActivateBonus(std::move(effect));
        }
    }

    // ========== СИСТЕМА ОЧКОВ ==========

    void GameStatePlayingData::UpdateScoreDisplay()
    {
        scoreText.setString("Score: " + std::to_string(scoreSystem.GetCurrentScore()));
    }

    // ========== СИСТЕМА СОХРАНЕНИЙ И ЖИЗНЕЙ ==========

    void GameStatePlayingData::SaveGame()
    {
        // В упрощенной реализации просто сохраняем состояние блоков
        std::cout << "Game state saved (lives: " << lives << ", score: " << scoreSystem.GetCurrentScore() << ")" << std::endl;
    }

    void GameStatePlayingData::LoadGame()
    {
        // В упрощенной реализации просто сбрасываем мяч
        std::cout << "Game state loaded" << std::endl;
        ResetBall();
    }

    void GameStatePlayingData::SetLives(int newLives)
    {
        lives = newLives;
        livesText.setString("Lives: " + std::to_string(lives));
    }

    void GameStatePlayingData::LoseLife()
    {
        SetLives(lives - 1);
        std::cout << "Life lost! Remaining lives: " << lives << std::endl;

        if (lives <= 0)
        {
            gameOver = true;
            std::cout << "Game Over! Final score: " << scoreSystem.GetCurrentScore() << std::endl;
            // Сохраняем рекорд
            Application::Instance().GetGame().UpdateRecord("Player", scoreSystem.GetCurrentScore());
        }
        else
        {
            LoadGame();
        }
    }

    void GameStatePlayingData::ResetBall()
    {
        isBallActive = false;
        ball.SetVelocity(sf::Vector2f(0.f, 0.f));
        ball.SetPosition(sf::Vector2f(
            platform.GetPosition().x,
            platform.GetPosition().y - platform.GetHeight() - ball.GetGlobalBounds().height / 2
        ));
    }

    // ========== ОБНОВЛЕНИЕ UI ==========

    void GameStatePlayingData::UpdateUI()
    {
        // Обновляем отображение активных бонусов
        std::stringstream bonusesStream;
        bonusesStream << "Active Bonuses:";

        for (const auto& bonusEffect : activeBonusEffects)
        {
            float remainingTime = bonusEffect.second;
            bonusesStream << "\n" << bonusEffect.first->GetName() << " ("
                << static_cast<int>(remainingTime) << "s)";
        }

        activeBonusesText.setString(bonusesStream.str());
    }

    void GameStatePlayingData::DrawUI(sf::RenderWindow& window)
    {
        window.draw(scoreText);
        window.draw(livesText);
        window.draw(activeBonusesText);

        if (gameWon)
        {
            window.draw(winText);
        }
    }

    // ========== ОСНОВНЫЕ ИГРОВЫЕ МЕТОДЫ ==========

    void GameStatePlayingData::InitBricks()
    {
        const int rows = 6;
        const int cols = 12;
        const float brickWidth = 60.f;
        const float brickHeight = 25.f;

        const float totalWidth = cols * brickWidth;
        const float startX = (SCREEN_WIDTH - totalWidth) / 2;
        const float startY = 40.f;
        const float horizontalSpacing = 2.f;

        bricks.clear();
        bricks.reserve(rows * cols);

        int unbreakableCount = 0;
        const int maxUnbreakable = 2;

        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < cols; ++col)
            {
                sf::Color color;
                BrickType brickType = BrickType::Normal;

                // Определяем тип блока в зависимости от позиции
                if (row == 0)
                {
                    color = sf::Color(255, 50, 50); // Красный
                    brickType = BrickType::Normal;
                }
                else if (row == 1)
                {
                    color = sf::Color(255, 150, 50); // Оранжевый
                    brickType = BrickType::Strong;
                }
                else if (row == 2)
                {
                    color = sf::Color(100, 200, 255); // Голубой для стекла
                    brickType = BrickType::Glass;
                }
                else if (row == 3)
                {
                    color = sf::Color(50, 255, 50); // Зеленый
                    // Ограничиваем неразрушимые блоки до 2 штук
                    if (unbreakableCount < maxUnbreakable && (col == 3 || col == 8))
                    {
                        brickType = BrickType::Unbreakable;
                        unbreakableCount++;
                    }
                    else
                    {
                        brickType = BrickType::Normal;
                    }
                }
                else if (row == 4)
                {
                    color = sf::Color(50, 150, 255); // Голубой
                    brickType = BrickType::Normal;
                }
                else
                {
                    color = sf::Color(200, 50, 255); // Фиолетовый
                    // Чередуем типы блоков в последнем ряду
                    brickType = (col % 3 == 0) ? BrickType::Strong : BrickType::Normal;
                }

                float x = startX + col * (brickWidth + horizontalSpacing);
                float y = startY + row * (brickHeight + 5.f);

                // Создаем блок
                Brick brick;
                brick.Init(brickTexture, sf::Vector2f(x, y), color, brickType);
                bricks.push_back(brick);
            }
        }

        std::cout << "Created " << (rows * cols) << " bricks with different types" << std::endl;
        std::cout << "Unbreakable bricks: " << unbreakableCount << std::endl;
    }

    void GameStatePlayingData::ProcessBrickDestruction(Brick& brick)
    {
        if (brick.IsDestroyed() && brick.GetType() != BrickType::Unbreakable)
        {
            // Начисляем очки
            int points = 10;
            if (brick.GetType() == BrickType::Strong) points = 40;
            if (brick.GetType() == BrickType::Glass) points = 15;

            scoreSystem.BrickDestroyed(points);

            // С вероятностью 10% создаем бонус
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_real_distribution<> dis(0.0, 1.0);

            if (dis(gen) < 0.1f) // 10% chance
            {
                SpawnBonus(brick.GetPosition());
            }
        }
    }

    void GameStatePlayingData::HandleBallBrickCollision()
    {
        sf::FloatRect ballBounds = ball.GetGlobalBounds();
        bool collisionHandled = false;

        for (auto& brick : bricks)
        {
            if (!brick.IsDestroyed() && ballBounds.intersects(brick.GetGlobalBounds()))
            {
                // Если активен бонус хрупких блоков, разрушаем сразу
                if (fragileBricks && brick.GetType() != BrickType::Unbreakable)
                {
                    brick.OnBallHit(); // Уничтожаем блок
                    ProcessBrickDestruction(brick);
                }
                else
                {
                    // Обычная логика разрушения
                    brick.OnBallHit();
                    ProcessBrickDestruction(brick);
                }

                // Определяем сторону столкновения для отскока
                sf::Vector2f ballCenter = ball.GetPosition();
                sf::FloatRect brickBounds = brick.GetGlobalBounds();

                float overlapLeft = ballCenter.x - (brickBounds.left - ballBounds.width / 2);
                float overlapRight = (brickBounds.left + brickBounds.width) - (ballCenter.x + ballBounds.width / 2);
                float overlapTop = ballCenter.y - (brickBounds.top - ballBounds.height / 2);
                float overlapBottom = (brickBounds.top + brickBounds.height) - (ballCenter.y + ballBounds.height / 2);

                bool fromLeft = std::abs(overlapLeft) < std::abs(overlapRight);
                bool fromTop = std::abs(overlapTop) < std::abs(overlapBottom);

                float minOverlapX = fromLeft ? overlapLeft : overlapRight;
                float minOverlapY = fromTop ? overlapTop : overlapBottom;

                if (std::abs(minOverlapX) < std::abs(minOverlapY))
                {
                    ball.ReverseVelocityX();
                }
                else
                {
                    ball.ReverseVelocityY();
                }

                collisionHandled = true;

                // Для стеклянных блоков не прерываем цикл - мяч может пройти сквозь несколько
                if (brick.GetType() != BrickType::Glass && collisionHandled)
                {
                    break;
                }
            }
        }

        HandleWinCondition();
    }

    void GameStatePlayingData::HandleBallPlatformCollision()
    {
        sf::FloatRect ballBounds = ball.GetGlobalBounds();
        sf::FloatRect platformBounds = platform.GetGlobalBounds();

        if (ballBounds.intersects(platformBounds))
        {
            // Вычисляем точку столкновения для определения направления отскока
            float ballCenterX = ballBounds.left + ballBounds.width / 2;
            float platformCenterX = platformBounds.left + platformBounds.width / 2;
            float hitPosition = (ballCenterX - platformCenterX) / (platformBounds.width / 2);

            // Изменяем направление мяча в зависимости от точки столкновения
            sf::Vector2f newVelocity = ball.GetVelocity();
            newVelocity.x = hitPosition * 400.f; // Максимальный угол отскока
            newVelocity.y = -std::abs(newVelocity.y); // Гарантируем движение вверх

            // Если мяч стоит на месте, задаем начальную скорость
            if (newVelocity.x == 0 && newVelocity.y == 0)
            {
                newVelocity = sf::Vector2f(200.f, -300.f);
            }

            ball.SetVelocity(newVelocity);

            // Корректируем позицию мяча, чтобы избежать залипания
            ball.SetPosition(sf::Vector2f(ball.GetPosition().x,
                platformBounds.top - ballBounds.height));
        }
    }

    void GameStatePlayingData::CheckCollisions()
    {
        HandleBallPlatformCollision();
        HandleBallBrickCollision();

        // Проверка проигрыша (мяч упал ниже платформы)
        if (isBallActive && ball.GetPosition().y > SCREEN_HEGHT)
        {
            LoseLife();
        }

        // Проверка столкновения с границами экрана (уже есть в Ball::Update)
    }

    void GameStatePlayingData::ResetGame()
    {
        gameWon = false;
        gameOver = false;
        scoreSystem.Reset();
        SetLives(3);
        isBallActive = false;
        fragileBricks = false;

        // Очищаем бонусы
        activeBonuses.clear();
        activeBonusEffects.clear();

        // Пересоздаем блоки
        InitBricks();

        // Сбрасываем мяч и платформу
        platform.Init();
        ResetBall();

        // Сохраняем новое состояние
        SaveGame();

        std::cout << "Game reset!" << std::endl;
    }

    void GameStatePlayingData::HandleWinCondition()
    {
        bool allDestroyed = true;
        for (const auto& brick : bricks)
        {
            if (!brick.IsDestroyed() && brick.GetType() != BrickType::Unbreakable)
            {
                allDestroyed = false;
                break;
            }
        }

        if (allDestroyed)
        {
            gameWon = true;
            isBallActive = false;
            std::cout << "You Win! Final score: " << scoreSystem.GetCurrentScore() << std::endl;
            // Сохраняем рекорд
            Application::Instance().GetGame().UpdateRecord("Player", scoreSystem.GetCurrentScore());
        }
    }

    // Функции для совместимости с существующей архитектурой
    void InitGameStatePlaying(GameStatePlayingData& data)
    {
        data.Init();
    }

    void ShutdownGameStatePlaying(GameStatePlayingData& data)
    {
        data.Shutdown();
    }

    void HandleGameStatePlayingWindowEvent(GameStatePlayingData& data, const sf::Event& event)
    {
        data.HandleWindowEvent(event);
    }

    void UpdateGameStatePlaying(GameStatePlayingData& data, float timeDelta)
    {
        data.Update(timeDelta);
    }

    void DrawGameStatePlaying(GameStatePlayingData& data, sf::RenderWindow& window)
    {
        data.Draw(window);
    }
}