#pragma once

#include "ecs/components.hpp"
#include "ecs/manager.hpp"
#include "ecs/types.hpp"

namespace ecs {


class AnimationSystem {
public:
    void update(ecs::Manager& manager, float deltaTime);
private:
    void updateTween(ecs::Manager& manager, EntityId entityId, Animation::TweenData& tween);
};

}