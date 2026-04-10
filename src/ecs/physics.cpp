#include "ecs/physics.hpp"
#include <algorithm>

ecs::PhysicsSystem::PhysicsSystem() {
    setWorld(worldWidth_, worldHeight_, cellSize_);
}

void ecs::PhysicsSystem::update(ecs::Manager& manager, float deltaTime) {
    std::vector<EntityId> entities = manager.getActiveEntities();
    updateCollisionPairs(manager, entities);
    
    float halfW = worldWidth_ / 2.0f;
    float halfH = worldHeight_ / 2.0f;

    for (ecs::EntityId id : entities) {
        if (!isDynamicEntity(manager, id)) continue;

        if (manager.hasComponent<ecs::CollisionState>(id)) {
            ecs::CollisionState& state = manager.getComponent<ecs::CollisionState>(id);
            state.onGround = state.onLeftWall = state.onRightWall = state.onCeiling = false;
        }

        ecs::Transform& transform = manager.getComponent<ecs::Transform>(id);
        ecs::Velocity& velocity = manager.getComponent<ecs::Velocity>(id);
        ecs::RigidBody& body = manager.getComponent<ecs::RigidBody>(id);

        velocity.y += body.gravity * deltaTime;
        velocity.x *= (1.0f - body.friction * deltaTime);
        velocity.x = std::clamp(velocity.x, -velocity.maxSpeed_x, velocity.maxSpeed_x);
        velocity.y = std::clamp(velocity.y, -velocity.maxSpeed_y, velocity.maxSpeed_y);
        
        // X Movement & Collision
        transform.x += velocity.x * deltaTime;     
        transform.x = std::clamp(transform.x, -halfW, halfW - body.width);

        updateNearbyEntities(manager, id);
        for (ecs::EntityId otherId : nearby_) {
            if (manager.hasComponent<ecs::RigidBody>(otherId)) {
                if (checkCollision(manager, id, otherId)) resolveCollisionX(manager, id, otherId);
            }
        }

        // Y Movement & Collision
        transform.y += velocity.y * deltaTime;
        transform.y = std::clamp(transform.y, -halfH, halfH - body.height);

        updateNearbyEntities(manager, id);
        for (ecs::EntityId otherId : nearby_) {
            if (manager.hasComponent<ecs::RigidBody>(otherId)) {
                if (checkCollision(manager, id, otherId)) resolveCollisionY(manager, id, otherId);
            }
        }
    }
}

// Helper to convert World Space (-halfW to +halfW) to Grid Space (0 to gridCols)
int ecs::PhysicsSystem::getGridIndex(float x, float y) const {
    int col = static_cast<int>((x + (worldWidth_ / 2.0f)) / cellSize_);
    int row = static_cast<int>((y + (worldHeight_ / 2.0f)) / cellSize_);
    
    col = std::clamp(col, 0, gridCols_ - 1);
    row = std::clamp(row, 0, gridRows_ - 1);
    
    return row * gridCols_ + col;
}

void ecs::PhysicsSystem::updateCollisionPairs(ecs::Manager& manager, const std::vector<EntityId>& entities) {
    for (auto& cell : spatialGrid_) cell.clear();

    for (ecs::EntityId id : entities) {
        if (manager.hasComponent<ecs::RigidBody>(id) && manager.hasComponent<ecs::Transform>(id)) {
            const auto& body = manager.getComponent<ecs::RigidBody>(id);
            if (!body.collidable) continue;

            const auto& transform = manager.getComponent<ecs::Transform>(id);

            float halfW = worldWidth_ / 2.0f;
            float halfH = worldHeight_ / 2.0f;

            int startCol = std::clamp(static_cast<int>((transform.x + halfW) / cellSize_), 0, gridCols_ - 1);
            int startRow = std::clamp(static_cast<int>((transform.y + halfH) / cellSize_), 0, gridRows_ - 1);
            int endCol = std::clamp(static_cast<int>((transform.x + body.width + halfW) / cellSize_), 0, gridCols_ - 1);
            int endRow = std::clamp(static_cast<int>((transform.y + body.height + halfH) / cellSize_), 0, gridRows_ - 1);

            for (int r = startRow; r <= endRow; ++r) {
                for (int c = startCol; c <= endCol; ++c) {
                    spatialGrid_[r * gridCols_ + c].push_back(id);
                }
            }
        }
    }
}

void ecs::PhysicsSystem::updateNearbyEntities(ecs::Manager& manager, ecs::EntityId id) {
    nearby_.clear();
    if (!manager.hasComponent<ecs::Transform>(id) || !manager.hasComponent<ecs::RigidBody>(id)) return;

    const auto& transform = manager.getComponent<ecs::Transform>(id);
    const auto& body = manager.getComponent<ecs::RigidBody>(id);

    float halfW = worldWidth_ / 2.0f;
    float halfH = worldHeight_ / 2.0f;

    int startCol = std::clamp(static_cast<int>((transform.x + halfW) / cellSize_), 0, gridCols_ - 1);
    int startRow = std::clamp(static_cast<int>((transform.y + halfH) / cellSize_), 0, gridRows_ - 1);
    int endCol = std::clamp(static_cast<int>((transform.x + body.width + halfW) / cellSize_), 0, gridCols_ - 1);
    int endRow = std::clamp(static_cast<int>((transform.y + body.height + halfH) / cellSize_), 0, gridRows_ - 1);

    for (int r = startRow; r <= endRow; ++r) {
        for (int c = startCol; c <= endCol; ++c) {
            const auto& cell = spatialGrid_[r * gridCols_ + c];
            for (EntityId otherId : cell) {
                if (otherId != id) nearby_.push_back(otherId);
            }
        }
    }

    std::sort(nearby_.begin(), nearby_.end());
    nearby_.erase(std::unique(nearby_.begin(), nearby_.end()), nearby_.end());
}

void ecs::PhysicsSystem::resolveCollisionX(ecs::Manager& manager, ecs::EntityId dynamic_ent, ecs::EntityId other) {
    auto& dynTrans = manager.getComponent<ecs::Transform>(dynamic_ent);
    auto& dynVel = manager.getComponent<ecs::Velocity>(dynamic_ent);
    const auto& dynBody = manager.getComponent<ecs::RigidBody>(dynamic_ent);
    const auto& othTrans = manager.getComponent<ecs::Transform>(other);
    const auto& othBody = manager.getComponent<ecs::RigidBody>(other);

    if (dynVel.x > 0.0f) {
        dynTrans.x = othTrans.x - dynBody.width - 0.001f;
        if (manager.hasComponent<ecs::CollisionState>(dynamic_ent)) manager.getComponent<ecs::CollisionState>(dynamic_ent).onRightWall = true;
    } else {
        dynTrans.x = othTrans.x + othBody.width + 0.001f;
        if (manager.hasComponent<ecs::CollisionState>(dynamic_ent)) manager.getComponent<ecs::CollisionState>(dynamic_ent).onLeftWall = true;
    }
    dynVel.x *= -dynBody.bounce;
}

void ecs::PhysicsSystem::resolveCollisionY(ecs::Manager& manager, ecs::EntityId dynamic_ent, ecs::EntityId other) {
    auto& dynTrans = manager.getComponent<ecs::Transform>(dynamic_ent);
    auto& dynVel = manager.getComponent<ecs::Velocity>(dynamic_ent);
    const auto& dynBody = manager.getComponent<ecs::RigidBody>(dynamic_ent);
    const auto& othTrans = manager.getComponent<ecs::Transform>(other);
    const auto& othBody = manager.getComponent<ecs::RigidBody>(other);

    if (dynVel.y > 0.0f) {
        dynTrans.y = othTrans.y - dynBody.height - 0.001f;
        if (manager.hasComponent<ecs::CollisionState>(dynamic_ent)) manager.getComponent<ecs::CollisionState>(dynamic_ent).onGround = true;
    } else {
        dynTrans.y = othTrans.y + othBody.height + 0.001f;
        if (manager.hasComponent<ecs::CollisionState>(dynamic_ent)) manager.getComponent<ecs::CollisionState>(dynamic_ent).onCeiling = true;
    }
    dynVel.y *= -dynBody.bounce;
}

void ecs::PhysicsSystem::setWorld(int width, int height, int cellSize) {
    worldWidth_ = width;
    worldHeight_ = height;
    cellSize_ = cellSize;
    gridCols_ = std::max(1, (int)std::ceil((float)worldWidth_ / cellSize_));
    gridRows_ = std::max(1, (int)std::ceil((float)worldHeight_ / cellSize_));
    spatialGrid_.assign(gridCols_ * gridRows_, {});
}

bool ecs::PhysicsSystem::checkCollision(ecs::Manager& manager, ecs::EntityId ent_a, ecs::EntityId ent_b) {
    const auto& bA = manager.getComponent<ecs::RigidBody>(ent_a);
    const auto& bB = manager.getComponent<ecs::RigidBody>(ent_b);
    if (!bA.collidable || !bB.collidable) return false;

    const auto& tA = manager.getComponent<ecs::Transform>(ent_a);
    const auto& tB = manager.getComponent<ecs::Transform>(ent_b);
    
    return (tA.x < tB.x + bB.width && tA.x + bA.width > tB.x &&
            tA.y < tB.y + bB.height && tA.y + bA.height > tB.y);
}

bool ecs::PhysicsSystem::isDynamicEntity(ecs::Manager& manager, ecs::EntityId id) {
    if (!manager.hasComponent<ecs::RigidBody>(id)) return false;
    const auto& body = manager.getComponent<ecs::RigidBody>(id);
    return (!body.isStatic && manager.hasComponent<ecs::Velocity>(id) && manager.hasComponent<ecs::Transform>(id));
}