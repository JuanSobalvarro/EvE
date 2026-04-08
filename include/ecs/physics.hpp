#pragma once

#include <vector>
#include <cmath>
#include <iostream>

#include "ecs/manager.hpp"
#include "ecs/components.hpp"
#include "ecs/types.hpp"

namespace ecs {

class PhysicsSystem {

public:
    void update(ecs::Manager& manager, float deltaTime);
        
private:
    bool checkCollision(ecs::Manager& manager, ecs::EntityId ent_a, ecs::EntityId ent_b);

    void resolveCollisionX(ecs::Manager& manager, ecs::EntityId dynamic_ent, ecs::EntityId other);
    void resolveCollisionY(ecs::Manager& manager, ecs::EntityId dynamic_ent, ecs::EntityId other);
};

}