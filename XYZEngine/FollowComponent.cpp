#include "pch.h"
#include "FollowComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SpriteRendererComponent.h"
#include <iostream>
#include <cmath>  // ƒл€ std::sqrt

namespace XYZEngine
{
    FollowComponent::FollowComponent(GameObject* gameObject)
        : Component(gameObject)
    {
        transform = gameObject->GetComponent<TransformComponent>();
        spriteRenderer = gameObject->GetComponent<SpriteRendererComponent>();

        if (transform == nullptr)
        {
            std::cout << "FollowComponent requires a TransformComponent." << std::endl;
            gameObject->RemoveComponent(this);
        }
    }

    void FollowComponent::SetTarget(GameObject* targetObject)
    {
        if (targetObject)
        {
            targetTransform = targetObject->GetComponent<TransformComponent>();
            if (targetTransform == nullptr)
            {
                std::cout << "Target object has no TransformComponent." << std::endl;
            }
        }
        else
        {
            targetTransform = nullptr;
        }
    }

    void FollowComponent::Update(float deltaTime)
    {
        if (transform == nullptr || targetTransform == nullptr) return;

        Vector2Df currentPos = transform->GetWorldPosition();
        Vector2Df targetPos = targetTransform->GetWorldPosition();
        Vector2Df direction = targetPos - currentPos;

        float length = direction.GetLength();

        if (length > 0.001f)
        {
            // –учна€ нормализаци€ вектора
            Vector2Df normalized(direction.x / length, direction.y / length);
            transform->MoveBy(normalized * speed * deltaTime);

            if (spriteRenderer != nullptr)
            {
                // ѕереворачиваем спрайт в зависимости от направлени€
                spriteRenderer->FlipX(direction.x < 0);
            }
        }
    }

    void FollowComponent::Render()
    {
        // Ётот метод может остатьс€ пустым, если не требуетс€ специальна€ отрисовка
    }

    void FollowComponent::SetSpeed(float newSpeed)
    {
        speed = newSpeed;
    }

    float FollowComponent::GetSpeed() const
    {
        return speed;
    }
}