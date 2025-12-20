#pragma once

#include "GameObject.h"
#include "Vector.h"

namespace XYZRoguelike
{
    class AI
    {
    public:
        AI(const XYZEngine::Vector2Df& position, XYZEngine::GameObject* player);
        ~AI();

        XYZEngine::GameObject* GetGameObject() const;

    private:
        XYZEngine::GameObject* gameObject;
        XYZEngine::GameObject* player;
    };
}
