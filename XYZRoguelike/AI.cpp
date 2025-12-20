#include "AI.h"
#include "GameWorld.h"
#include "TransformComponent.h"
#include "SpriteRendererComponent.h"
#include "FollowComponent.h"
#include "RigidbodyComponent.h"
#include "SpriteColliderComponent.h"
#include "ResourceSystem.h"
#include <iostream>

namespace XYZRoguelike
{
    AI::AI(const XYZEngine::Vector2Df& position, XYZEngine::GameObject* player)
        : player(player)
    {
        // Получаем экземпляр GameWorld
        XYZEngine::GameWorld* gameWorld = XYZEngine::GameWorld::Instance();
        if (!gameWorld)
        {
            std::cerr << "GameWorld instance is null!" << std::endl;
            return;
        }

        // Создаем игровой объект для AI
        gameObject = gameWorld->CreateGameObject("AI");
        if (!gameObject)
        {
            std::cerr << "Failed to create AI GameObject!" << std::endl;
            return;
        }

        // Устанавливаем позицию
        auto transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
        if (transform)
        {
            transform->SetWorldPosition(position);
        }
        else
        {
            std::cerr << "AI GameObject has no TransformComponent!" << std::endl;
        }

        // Добавляем компонент для отрисовки
        auto render = gameObject->AddComponent<XYZEngine::SpriteRendererComponent>();
        if (render)
        {
            // Получаем текстуру из ResourceSystem
            auto resourceSystem = XYZEngine::ResourceSystem::Instance();
            if (resourceSystem)
            {
                auto texture = resourceSystem->GetTextureMapElementShared("AI", 0);
                if (texture)
                {
                    render->SetTexture(*texture);
                    render->SetPixelSize(100, 100);
                }
                else
                {
                    std::cerr << "Failed to get AI texture!" << std::endl;
                }
            }
        }

        // Добавляем компонент следования за игроком
        if (player)
        {
            auto follower = gameObject->AddComponent<XYZEngine::FollowComponent>();
            if (follower)
            {
                follower->SetTarget(player);
                follower->SetSpeed(120.0f);
            }
        }

        // Добавляем физический компонент
        auto rigidbody = gameObject->AddComponent<XYZEngine::RigidbodyComponent>();
        if (rigidbody)
        {
            rigidbody->SetKinematic(false);
        }

        // Добавляем компонент коллайдера (исправлено: ganeObject -> gameObject)
        auto collider = gameObject->AddComponent<XYZEngine::SpriteColliderComponent>();
        if (!collider)
        {
            std::cerr << "Failed to add SpriteColliderComponent to AI!" << std::endl;
        }
    }

    AI::~AI()
    {
        // Деструктор: при необходимости можно добавить очистку ресурсов
    }

    XYZEngine::GameObject* AI::GetGameObject() const
    {
        return gameObject;
    }
}