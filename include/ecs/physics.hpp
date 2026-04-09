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
    PhysicsSystem();

    void update(ecs::Manager& manager, float deltaTime);

    void setWorldSize(int width, int height);
        
private:
    bool checkCollision(ecs::Manager& manager, ecs::EntityId ent_a, ecs::EntityId ent_b);

    void resolveCollisionX(ecs::Manager& manager, ecs::EntityId dynamic_ent, ecs::EntityId other);
    void resolveCollisionY(ecs::Manager& manager, ecs::EntityId dynamic_ent, ecs::EntityId other);

    void updateCollisionPairs(ecs::Manager& manager, const std::vector<EntityId>& entities);
    std::vector<EntityId> getNearbyEntities(ecs::Manager& manager, ecs::EntityId id);

    int cellSize_ = 64;
    int worldWidth_ = 1000;
    int worldHeight_ = 1000;
    int gridCols_ = 0;
    int gridRows_ = 0;
    std::vector<std::vector<EntityId>> spatialGrid_;
};

}