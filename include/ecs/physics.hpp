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

    void setWorld(int width, int height, int cellSize);
        
private:
    bool checkCollision(ecs::Manager& manager, ecs::EntityId ent_a, ecs::EntityId ent_b);

    void resolveCollisionX(ecs::Manager& manager, ecs::EntityId dynamic_ent, ecs::EntityId other);
    void resolveCollisionY(ecs::Manager& manager, ecs::EntityId dynamic_ent, ecs::EntityId other);

    void updateCollisionPairs(ecs::Manager& manager, const std::vector<EntityId>& entities);
    void updateNearbyEntities(ecs::Manager& manager, ecs::EntityId id);

    int cellSize_ = 100;
    int worldWidth_ = 1000;
    int worldHeight_ = 1000;
    int gridCols_ = 0;
    int gridRows_ = 0;
    std::vector<std::vector<EntityId>> spatialGrid_;
    std::unordered_map<EntityId, size_t> entityToGridIndex_;

    std::vector<ecs::EntityId> nearby_; // reusable vector to avoid allocations in updateNearbyEntities

    bool isDynamicEntity(ecs::Manager& manager, ecs::EntityId id);

};

}