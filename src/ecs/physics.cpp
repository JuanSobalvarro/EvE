#include "ecs/physics.hpp"

ecs::PhysicsSystem::PhysicsSystem() {
    setWorldSize(worldWidth_, worldHeight_);
}

void ecs::PhysicsSystem::update(ecs::Manager& manager, float deltaTime) {
    std::vector<EntityId> entities = manager.getActiveEntities();

    updateCollisionPairs(manager, entities);
    
    // update each dynamic entity that needs to check collision and move accordingly
    for (ecs::EntityId id : entities) {
        if (!isDynamicEntity(manager, id)) {
            continue;
        }

        if (manager.hasComponent<ecs::CollisionState>(id)) {
            ecs::CollisionState& state = manager.getComponent<ecs::CollisionState>(id);
            state.onGround = false;
            state.onLeftWall = false;
            state.onRightWall = false;
            state.onCeiling = false;
        }

        ecs::Transform& transform = manager.getComponent<ecs::Transform>(id);
        ecs::Velocity& velocity = manager.getComponent<ecs::Velocity>(id);
        ecs::RigidBody& body = manager.getComponent<ecs::RigidBody>(id);

        // apply gravity
        velocity.y += body.gravity * deltaTime;
        velocity.x *= (1.0f - body.friction * deltaTime); // TODO: friction should be dependent of collision state

        // clamp velocity to max speed
        velocity.x = std::clamp(velocity.x, -velocity.maxSpeed_x, velocity.maxSpeed_x);
        velocity.y = std::clamp(velocity.y, -velocity.maxSpeed_y, velocity.maxSpeed_y);
        
        // check first x
        transform.x += velocity.x * deltaTime;
        transform.x = std::clamp(transform.x, 0.0f, static_cast<float>(worldWidth_));

        // now check for neighbors
        std::vector<EntityId> neighbors = getNearbyEntities(manager, id);
        for (ecs::EntityId otherId : neighbors) {
            if (otherId != id && manager.hasComponent<ecs::RigidBody>(otherId)) {
                if (checkCollision(manager, id, otherId)) {
                    resolveCollisionX(manager, id, otherId);
                }
            }
        }

        // check second y
        transform.y += velocity.y * deltaTime;
        transform.y = std::clamp(transform.y, 0.0f, static_cast<float>(worldHeight_));

        // refetch neighbors in case we moved to a different cell
        neighbors = getNearbyEntities(manager, id);
        for (ecs::EntityId otherId : neighbors) {
            if (otherId != id && manager.hasComponent<ecs::RigidBody>(otherId)) {
                if (checkCollision(manager, id, otherId)) {
                    resolveCollisionY(manager, id, otherId);
                }
            }
        }
    }
}

bool ecs::PhysicsSystem::checkCollision(ecs::Manager& manager, ecs::EntityId ent_a, ecs::EntityId ent_b) {
    const ecs::RigidBody& body_a = manager.getComponent<ecs::RigidBody>(ent_a);
    const ecs::RigidBody& body_b = manager.getComponent<ecs::RigidBody>(ent_b);
    const ecs::Transform& transform_a = manager.getComponent<ecs::Transform>(ent_a);
    const ecs::Transform& transform_b = manager.getComponent<ecs::Transform>(ent_b);
    
    if (!body_a.collidable || !body_b.collidable) {
        return false;
    }
    
    bool collisionX = transform_a.x < transform_b.x + body_b.width &&
                      transform_a.x + body_a.width > transform_b.x;
    bool collisionY = transform_a.y < transform_b.y + body_b.height &&
                      transform_a.y + body_a.height > transform_b.y;

    return collisionX && collisionY;
}

void ecs::PhysicsSystem::resolveCollisionX(ecs::Manager& manager, ecs::EntityId dynamic_ent, ecs::EntityId other) {
    ecs::Transform& dynTrans = manager.getComponent<ecs::Transform>(dynamic_ent);
    ecs::Velocity& dynVel = manager.getComponent<ecs::Velocity>(dynamic_ent);
    const ecs::RigidBody& dynBody = manager.getComponent<ecs::RigidBody>(dynamic_ent);

    const ecs::Transform& othTrans = manager.getComponent<ecs::Transform>(other);
    const ecs::RigidBody& othBody = manager.getComponent<ecs::RigidBody>(other);

    if (dynVel.x > 0.0f) {
        dynTrans.x = othTrans.x - dynBody.width;
        if (manager.hasComponent<ecs::CollisionState>(dynamic_ent)) {
            manager.getComponent<ecs::CollisionState>(dynamic_ent).onRightWall = true;
        }
    } else if (dynVel.x < 0.0f) {
        dynTrans.x = othTrans.x + othBody.width;
        if (manager.hasComponent<ecs::CollisionState>(dynamic_ent)) {
            manager.getComponent<ecs::CollisionState>(dynamic_ent).onLeftWall = true;
        }
    }
    dynVel.x = dynBody.mass * dynVel.x * dynBody.bounce * -1.0f; // simple bounce effect
}

void ecs::PhysicsSystem::resolveCollisionY(ecs::Manager& manager, ecs::EntityId dynamic_ent, ecs::EntityId other) {
    ecs::Transform& dynTrans = manager.getComponent<ecs::Transform>(dynamic_ent);
    ecs::Velocity& dynVel = manager.getComponent<ecs::Velocity>(dynamic_ent);
    const ecs::RigidBody& dynBody = manager.getComponent<ecs::RigidBody>(dynamic_ent);

    const ecs::Transform& othTrans = manager.getComponent<ecs::Transform>(other);
    const ecs::RigidBody& othBody = manager.getComponent<ecs::RigidBody>(other);

    if (dynVel.y > 0.0f) {
        dynTrans.y = othTrans.y - dynBody.height;
        if (manager.hasComponent<ecs::CollisionState>(dynamic_ent)) {
            manager.getComponent<ecs::CollisionState>(dynamic_ent).onGround = true;
        }
    } else if (dynVel.y < 0.0f) {
        dynTrans.y = othTrans.y + othBody.height;
        if (manager.hasComponent<ecs::CollisionState>(dynamic_ent)) {
            manager.getComponent<ecs::CollisionState>(dynamic_ent).onCeiling = true;

        }
    }
    dynVel.y = dynBody.mass * dynVel.y * dynBody.bounce * -1.0f; // simple bounce effect
}

void ecs::PhysicsSystem::updateCollisionPairs(ecs::Manager& manager, const std::vector<EntityId>& entities) {
    for (auto& row : spatialGrid_) {
        row.clear();
    }

    // assign entities to grid cells
    for (ecs::EntityId id : entities) {
        if (manager.hasComponent<ecs::RigidBody>(id) && manager.hasComponent<ecs::Transform>(id)) {
            const ecs::RigidBody& body = manager.getComponent<ecs::RigidBody>(id);

            if (!body.collidable) {
                continue;
            }

            const ecs::Transform& transform = manager.getComponent<ecs::Transform>(id);

            int startCol = std::clamp(static_cast<int>(transform.x) / cellSize_, 0, gridCols_ - 1);
            int startRow = std::clamp(static_cast<int>(transform.y) / cellSize_, 0, gridRows_ - 1);
            
            int endCol = std::clamp(static_cast<int>(transform.x + body.width) / cellSize_, 0, gridCols_ - 1);
            int endRow = std::clamp(static_cast<int>(transform.y + body.height) / cellSize_, 0, gridRows_ - 1);

            // insert into every cell the entity touches
            for (int row = startRow; row <= endRow; ++row) {
                for (int col = startCol; col <= endCol; ++col) {
                    spatialGrid_[row * gridCols_ + col].push_back(id);
                }
            }
        }
    }
}

std::vector<ecs::EntityId> ecs::PhysicsSystem::getNearbyEntities(ecs::Manager& manager, ecs::EntityId id) {
    std::vector<EntityId> nearby;

    if (!manager.hasComponent<ecs::Transform>(id) || !manager.hasComponent<ecs::RigidBody>(id)) {
        return nearby;
    }

    ecs::Transform& transform = manager.getComponent<ecs::Transform>(id);
    ecs::RigidBody& body = manager.getComponent<ecs::RigidBody>(id);

    int startCol = std::clamp(static_cast<int>(transform.x) / cellSize_, 0, gridCols_ - 1);
    int startRow = std::clamp(static_cast<int>(transform.y) / cellSize_, 0, gridRows_ - 1);
    int endCol = std::clamp(static_cast<int>(transform.x + body.width) / cellSize_, 0, gridCols_ - 1);
    int endRow = std::clamp(static_cast<int>(transform.y + body.height) / cellSize_, 0, gridCols_ - 1);

    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            int index = row * gridCols_ + col;
            if (index >= 0 && index < spatialGrid_.size()) {
                for (EntityId otherId : spatialGrid_[index]) {
                    if (otherId != id) {
                        nearby.push_back(otherId);
                    }
                }
            }
        }
    }

    std::sort(nearby.begin(), nearby.end());
    nearby.erase(std::unique(nearby.begin(), nearby.end()), nearby.end());

    return nearby;
}

void ecs::PhysicsSystem::setWorldSize(int width, int height) {
    worldWidth_ = width;
    worldHeight_ = height;
    
    gridCols_ = std::ceil((float)worldWidth_ / cellSize_);
    gridRows_ = std::ceil((float)worldHeight_ / cellSize_);
    
    spatialGrid_.resize(gridCols_ * gridRows_);
}

bool ecs::PhysicsSystem::isDynamicEntity(ecs::Manager& manager, ecs::EntityId id) {
    if (!manager.hasComponent<ecs::RigidBody>(id))
        return false;

    const ecs::RigidBody& body = manager.getComponent<ecs::RigidBody>(id);
    if (body.isStatic)
        return false;

    // if it has no velocity or transform we cant move it so consider it non dynamic
    if (!manager.hasComponent<ecs::Velocity>(id) || !manager.hasComponent<ecs::Transform>(id))
        return false;
    
    return true;
}