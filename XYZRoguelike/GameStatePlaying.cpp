#include "GameStatePlaying.h"
#include "Application.h"
#include "Block.h"
#include "Game.h"
#include "Text.h"
#include "ThreeHitBlock.h"
#include "randomizer.h"

#include <assert.h>
#include <sstream>

namespace XYZRoguelike
{
	void GameStatePlayingData::Init()
	{	
		// Init game resources (terminate if error)
		assert(font.loadFromFile(SETTINGS.FONTS_PATH + "Roboto-Regular.ttf"));
		assert(gameOverSoundBuffer.loadFromFile(SETTINGS.SOUNDS_PATH + "Death.wav"));

		//factoriesInit
		factories.emplace(BlockType::Simple, std::make_unique<SimpleBlockFactory>());
		factories.emplace(BlockType::ThreeHit, std::make_unique<ThreeHitBlockFactory>());
		factories.emplace(BlockType::Unbreackable, std::make_unique<UnbreackableBlockFactory>());

		// Инициализация спаунера крипперов
		m_creeperSpawner = std::make_unique<CreeperSpawner>();

		// Init background
		background.setSize(sf::Vector2f(SETTINGS.SCREEN_WIDTH, SETTINGS.SCREEN_HEIGHT));
		background.setPosition(0.f, 0.f);
		background.setFillColor(sf::Color(0, 0, 0));

		scoreText.setFont(font);
		scoreText.setCharacterSize(24);
		scoreText.setFillColor(sf::Color::Yellow);

		inputHintText.setFont(font);
		inputHintText.setCharacterSize(24);
		inputHintText.setFillColor(sf::Color::White);
		inputHintText.setString("Use arrow keys to move, ESC to pause");
		inputHintText.setOrigin(GetTextOrigin(inputHintText, { 1.f, 0.f }));

		auto platform = std::make_shared<Platform>(sf::Vector2f({ SETTINGS.SCREEN_WIDTH / 2.f, SETTINGS.SCREEN_HEIGHT - SETTINGS.PLATFORM_HEIGHT / 2.f }));
		gameObjects.emplace_back(platform);		
		auto ball = std::make_shared<Ball>(sf::Vector2f({ SETTINGS.SCREEN_WIDTH / 2.f, SETTINGS.SCREEN_HEIGHT - SETTINGS.PLATFORM_HEIGHT - SETTINGS.BALL_SIZE / 2.f }));
		ball->AddObserver(weak_from_this());
		gameObjects.emplace_back(ball);

		createBlocks();

		auto platform = std::dynamic_pointer_cast<Platform>(gameObjects[0]);
		auto ball = std::dynamic_pointer_cast<Ball>(gameObjects[1]);

		// Init sounds
		gameOverSound.setBuffer(gameOverSoundBuffer);
		bonusSound.setBuffer(bonusSoundBuffer);
		bonuses.emplace(BonusType::BiggerPlatform, Bonus(
			[wplatform = std::weak_ptr<Platform>(platform), &sound = bonusSound]() {
			auto platform = wplatform.lock();
			if (platform) {
				sound.play();
				platform->ChangeWidth(2);
			}
		},
			[wplatform = std::weak_ptr<Platform>(platform)]() {
			auto platform = wplatform.lock();
			if (platform) {

				platform->ChangeWidth(0.5);
			}
		},
			SETTINGS.BONUS_DURATION
			));

		bonuses.emplace(BonusType::SlowBall, Bonus(
			[wball = std::weak_ptr<Ball>(ball), &sound = bonusSound]() {
			auto ball = wball.lock();
			if (ball) {
				sound.play();
				ball->ChangeSpeed(0.5);
			}
		},
			[wball = std::weak_ptr<Ball>(ball)]() {
			auto ball = wball.lock();
			if (ball) {
				ball->ChangeSpeed(1);
			}
		},
			SETTINGS.BONUS_DURATION
			));

		// Инициализация спаунера крипперов после создания игровых объектов
		m_creeperSpawner = std::make_unique<CreeperSpawner>();
		InitializeCreeperSystem();

	}

	void GameStatePlayingData::HandleWindowEvent(const sf::Event& event)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				Application::Instance().GetGame().PauseGame();
			}
		}
	}

	void GameStatePlayingData::Update(float timeDelta)
	{
		auto updateFunctor = [timeDelta](const auto& obj) {
			if (obj) obj->Update(timeDelta);
			};

		std::for_each(gameObjects.begin(), gameObjects.end(), updateFunctor);
		std::for_each(blocks.begin(), blocks.end(), updateFunctor);
		std::for_each(bonuses.begin(), bonuses.end(), [timeDelta](auto& bonusPair) {
			bonusPair.second.Update(timeDelta);
			});

		std::shared_ptr <Platform> platform = std::dynamic_pointer_cast<Platform>(gameObjects[0]);
		std::shared_ptr<Ball> ball = std::dynamic_pointer_cast<Ball>(gameObjects[1]);

		auto isCollision = platform->CheckCollision(ball);

		bool needInverseDirX = false;
		bool needInverseDirY = false;


		bool hasBrokeOneBlock = false;
		//remove-erase idiom
		blocks.erase(
			std::remove_if(blocks.begin(), blocks.end(),
				[ball, &hasBrokeOneBlock, &needInverseDirX, &needInverseDirY, this](auto block) {
					if ((!hasBrokeOneBlock) && block->CheckCollision(ball)) {
						hasBrokeOneBlock = true;
						const auto ballPos = ball->GetPosition();
						const auto blockRect = block->GetRect();

						GetBallInverse(ballPos, blockRect, needInverseDirX, needInverseDirY);
					}
					return block->IsBroken();
				}),
			blocks.end()
					);
		if (needInverseDirX) {
			ball->InvertDirectionX();
		}
		if (needInverseDirY) {
			ball->InvertDirectionY();
		}

		// Обновляем бонусы
		for (auto& [bonusType, bonus] : bonuses) {
			bonus.Update(timeDelta);
		}


		// Обновление крипперов
		if (m_creeperSpawner)
		{
			m_creeperSpawner->Update(timeDelta);
		}

		// Обновляем и проверяем крипперы
		for (auto& creeper : m_creepers)
		{
			if (creeper)
			{
				creeper->Update(timeDelta);

				// Проверка столкновения криппера с мячом
				auto ball = std::dynamic_pointer_cast<Ball>(gameObjects[1]);
				if (ball && creeper->CheckCollision(ball))
				{
					// Криппер получает урон
					creeper->TakeDamage(10);

					// Мяч отскакивает
					ball->InvertDirectionY();

					// Проверяем, уничтожен ли криппер
					if (creeper->IsDead())
					{
						// Увеличиваем счёт
						// ...
					}
				}
			}
		}

		// Удаляем уничтоженных крипперов
		auto it = std::remove_if(m_creepers.begin(), m_creepers.end(),
			[](const auto& creeper) {
				return !creeper || creeper->IsDead();
			});
		m_creepers.erase(it, m_creepers.end());
	}

	void GameStatePlayingData::Draw(sf::RenderWindow& window)
	{
		// Draw background
		window.draw(background);

		static auto drawFunc = [&window](auto block) { block->Draw(window); };

		// Отрисовка игровых объектов
		for (const auto& obj : gameObjects)
		{
			if (obj) obj->Draw(window);
		}

		// Отрисовка блоков
		for (const auto& block : blocks)
		{
			if (block) block->Draw(window);
		}

		// Отрисовка крипперов
		for (const auto& creeper : m_creepers)
		{
			if (creeper) creeper->Draw(window);
		}

		// Отрисовка UI
		scoreText.setOrigin(GetTextOrigin(scoreText, { 0.f, 0.f }));
		scoreText.setPosition(10.f, 10.f);
		window.draw(scoreText);

		sf::Vector2f viewSize = window.getView().getSize();
		inputHintText.setPosition(viewSize.x - 10.f, 10.f);
		window.draw(inputHintText);

		// Draw game objects
		std::for_each(gameObjects.begin(), gameObjects.end(), drawFunc);
		std::for_each(blocks.begin(), blocks.end(), drawFunc);

		scoreText.setOrigin(GetTextOrigin(scoreText, { 0.f, 0.f }));
		scoreText.setPosition(10.f, 10.f);
		window.draw(scoreText);

		sf::Vector2f viewSize = window.getView().getSize();
		inputHintText.setPosition(viewSize.x - 10.f, 10.f);
		window.draw(inputHintText);
	}

	void GameStatePlayingData::LoadNextLevel()
	{
		if (currentLevel >= levelLoder.GetLevelCount() - 1) {
			Game& game = Application::Instance().GetGame();
			game.WinGame();
		}
		else
		{
			auto platform = std::dynamic_pointer_cast<Platform>(gameObjects[0]);
			auto ball = std::dynamic_pointer_cast<Ball>(gameObjects[1]);

			if (platform) platform->restart();
			if (ball) ball->restart();

			blocks.clear();
			m_creepers.clear(); // Очищаем крипперов при смене уровня

			if (m_creeperSpawner)
			{
				m_creeperSpawner->Clear();
			}

			++currentLevel;
			createBlocks();
			InitializeCreeperSystem(); // Переинициализируем крипперов для нового уровня
		}


	}

	void GameStatePlayingData::createBlocks() 
	{
		for (const auto& pair : factories)
		{
			pair.second->ClearCounter();
		}
		auto self = weak_from_this();

		auto level = levelLoder.GetLevel(currentLevel);

		for (auto pairPosBlockTYpe : level.m_blocks)
		{
			auto blockType = pairPosBlockTYpe.second;
			sf::Vector2i pos = pairPosBlockTYpe.first;

			sf::Vector2f position{
				(float)(SETTINGS.BLOCK_SHIFT + SETTINGS.BLOCK_WIDTH / 2.f + pos.x * (SETTINGS.BLOCK_WIDTH + SETTINGS.BLOCK_SHIFT))
				, (float)pos.y * SETTINGS.BLOCK_HEIGHT
			};


			blocks.emplace_back(factories.at(blockType)->CreateBlock(position));
			blocks.back()->AddObserver(self);
		}


		for (const auto& pair : factories)
		{
			breackableBlocksCount += pair.second->GetcreatedBreackableBlocksCount();
		}
	}

	void GameStatePlayingData::GetBallInverse(const sf::Vector2f& ballPos, const sf::FloatRect& blockRect, bool& needInverseDirX, bool& needInverseDirY) {

		if (ballPos.y > blockRect.top + blockRect.height)
		{
			needInverseDirY = true;
		}
		if (ballPos.x < blockRect.left)
		{
			needInverseDirX = true;
		}
		if (ballPos.x > blockRect.left + blockRect.width)
		{
			needInverseDirX = true;
		}
	}

	void GameStatePlayingData::Notify(std::shared_ptr<IObservable> observable)
	{
		if (auto block = std::dynamic_pointer_cast<Block>(observable); block) {
			--breackableBlocksCount;
			Game& game = Application::Instance().GetGame();
			if (breackableBlocksCount == 0) {
				game.LoadNextLevel();
			}
			else
			{
				auto percent = random<int>(0, 100);
				if (SETTINGS.BONUS_PROPABILITY_PERCENT >= percent) {
					BonusType bonusType = (BonusType)(random<int>(0, (int)BonusType::Count - 1));
					bonuses.at(bonusType).Activate();
				}
			}
		}
		else if (auto ball = std::dynamic_pointer_cast<Ball>(observable); ball)
		{
			if (ball->GetPosition().y > gameObjects.front()->GetRect().top) {
				gameOverSound.play();
				Application::Instance().GetGame().LooseGame();
			}
		}
	}

	void GameStatePlayingData::InitializeCreeperSystem()
	{
		auto platform = std::dynamic_pointer_cast<Platform>(gameObjects[0]);

		if (platform && m_creeperSpawner)
		{
			CreeperSpawner::SpawnConfig config;
			config.minCreepers = 2;
			config.maxCreepers = 4;
			config.minSpeed = 100.0f;
			config.maxSpeed = 200.0f;
			config.minHealth = 50;
			config.maxHealth = 100;
			config.spawnRadius = 200.0f;

			// Устанавливаем функцию проверки позиции
			config.positionValidator = [this](const sf::Vector2f& pos) {
				return IsValidSpawnPosition(pos);
				};

			m_creeperSpawner->Initialize(platform, config);

			// Спауним крипперов и добавляем их в массив
			auto newCreepers = m_creeperSpawner->SpawnCreepers();
			m_creepers.insert(m_creepers.end(), newCreepers.begin(), newCreepers.end());
		}

}
