#include "ecs/physics.hpp"

ecs::PhysicsSystem::PhysicsSystem() {
    setWorldSize(worldWidth_, worldHeight_);
}

void ecs::PhysicsSystem::update(ecs::Manager& manager, float deltaTime) {
    std::vector<EntityId> entities = manager.getActiveEntities();

    updateCollisionPairs(manager, entities);
    
    for (ecs::EntityId id : entities) {
        if (!manager.hasComponent<RigidBody>(id) || manager.getComponent<RigidBody>(id).isStatic) {
            continue;
        }
        
        if (manager.hasComponent<ecs::CollisionState>(id)) {
            ecs::CollisionState& state = manager.getComponent<ecs::CollisionState>(id);
            state.onGround = false;
            state.onCeiling = false;
            state.onLeftWall = false;
            state.onRightWall = false;
        }
        
        ecs::Transform& transform = manager.getComponent<ecs::Transform>(id);
        ecs::Velocity& velocity = manager.getComponent<ecs::Velocity>(id);
        ecs::RigidBody& body = manager.getComponent<ecs::RigidBody>(id);
        
        velocity.y += body.gravity * deltaTime;
        velocity.x *= (1.0f - body.friction * deltaTime);
        
        if (velocity.x > velocity.maxSpeed_x) velocity.x = velocity.maxSpeed_x;
        if (velocity.x < -velocity.maxSpeed_x) velocity.x = -velocity.maxSpeed_x;
        if (velocity.y > velocity.maxSpeed_y) velocity.y = velocity.maxSpeed_y;
        if (velocity.y < -velocity.maxSpeed_y) velocity.y = -velocity.maxSpeed_y;
        
        transform.x += velocity.x * deltaTime;
        
        for (ecs::EntityId otherId : entities) {
            if (otherId != id && manager.hasComponent<ecs::RigidBody>(otherId)) {
                if (checkCollision(manager, id, otherId)) {
                    resolveCollisionX(manager, id, otherId);
                }
            }
        }
        
        transform.y += velocity.y * deltaTime;
        for (ecs::EntityId otherId : entities) {
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
    dynVel.x = 0.0f;
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
    dynVel.y = 0.0f;
}

void ecs::PhysicsSystem::updateCollisionPairs(ecs::Manager& manager, const std::vector<EntityId>& entities) {
    for (auto& row : spatialGrid_) {
        row.clear();
    }

    // assign entities to grid cells
    for (ecs::EntityId id : entities) {
        if (manager.hasComponent<ecs::RigidBody>(id) && manager.hasComponent<ecs::Transform>(id)) {
            const ecs::Transform& transform = manager.getComponent<ecs::Transform>(id);
            const ecs::RigidBody& body = manager.getComponent<ecs::RigidBody>(id);

            // if the entity is not collidable we can skip it
            if (!body.collidable) continue; 

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

void ecs::PhysicsSystem::setWorldSize(int width, int height) {
    worldWidth_ = width;
    worldHeight_ = height;
    
    gridCols_ = std::ceil((float)worldWidth_ / cellSize_);
    gridRows_ = std::ceil((float)worldHeight_ / cellSize_);
    
    spatialGrid_.resize(gridCols_ * gridRows_);
}