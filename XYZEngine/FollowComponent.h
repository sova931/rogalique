#pragma once

#include "Component.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SpriteRendererComponent.h"
#include "Vector.h"

namespace XYZEngine
{
    class FollowComponent : public Component  // Наследуем от Component
    {
    public:
        FollowComponent(GameObject* gameObject);
        ~FollowComponent() = default;

        void Update(float deltaTime) override;
        void Render() override;

        void SetTarget(GameObject* targetObject);  // Исправлено: GaneObject -> GameObject
        void SetSpeed(float newSpeed);
        float GetSpeed() const;

    private:
        TransformComponent* transform = nullptr;
        TransformComponent* targetTransform = nullptr;
        SpriteRendererComponent* spriteRenderer = nullptr;

        float speed = 100.f;
    };
}