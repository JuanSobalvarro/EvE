#include "scene/demo.hpp"
#include "core/game.hpp"

scene::Demo::Demo(ecs::Manager& ecsManager, assets::Manager& assetManager, core::Game& game) : Scene(ecsManager, assetManager, game) {
}

scene::Demo::~Demo() {
}

void scene::Demo::onEnter() {
    bgEntity_ = ecsManager_.createEntity();
    ecsManager_.addComponent<ecs::Transform>(bgEntity_, 0.0f, 0.0f);
    ecsManager_.addComponent<ecs::RigidBody>(bgEntity_, 800.0f, 600.0f, true, false);
    ecsManager_.addComponent<ecs::Shape>(bgEntity_, ecs::ShapeType::Rectangle, 800.0f, 600.0f, 0.0f, true);
    ecsManager_.addComponent<ecs::Color>(bgEntity_, (Uint8)255, (Uint8)255, (Uint8)255, (Uint8)255);

    floorEntity_ = ecsManager_.createEntity();
    ecsManager_.addComponent<ecs::Transform>(floorEntity_, 0.0f, 500.0f);
    ecsManager_.addComponent<ecs::RigidBody>(floorEntity_, 800.0f, 100.0f, true, true);
    ecsManager_.addComponent<ecs::Shape>(floorEntity_, ecs::ShapeType::Rectangle, 800.0f, 100.0f, 0.0f, true);
    ecsManager_.addComponent<ecs::Color>(floorEntity_, (Uint8)200, (Uint8)200, (Uint8)200, (Uint8)255);

    // external walls (out of screen)
    ecs::EntityId leftWall = ecsManager_.createEntity();
    ecsManager_.addComponent<ecs::Transform>(leftWall, -50.0f, 0.0f);
    ecsManager_.addComponent<ecs::RigidBody>(leftWall, 50.0f, 600.0f, true, true);
    ecsManager_.addComponent<ecs::Shape>(leftWall, ecs::ShapeType::Rectangle,
        50.0f, 600.0f, 0.0f, true);
    ecs::EntityId rightWall = ecsManager_.createEntity();
    ecsManager_.addComponent<ecs::Transform>(rightWall, 800.0f, 0.0f);
    ecsManager_.addComponent<ecs::RigidBody>(rightWall, 50.0f, 600.0f, true, true);
    ecsManager_.addComponent<ecs::Shape>(rightWall, ecs::ShapeType::Rectangle,
        50.0f, 600.0f, 0.0f, true);
    ecs::EntityId topWall = ecsManager_.createEntity();
    ecsManager_.addComponent<ecs::Transform>(topWall, 0.0f, -50.0f);
    ecsManager_.addComponent<ecs::RigidBody>(topWall, 800.0f, 50.0f, true, true);
    ecsManager_.addComponent<ecs::Shape>(topWall, ecs::ShapeType::Rectangle,
        800.0f, 50.0f, 0.0f, true);

    // player
    ernestEntity_ = ecsManager_.createEntity();
    ecsManager_.addComponent<ecs::Transform>(ernestEntity_, 400.0f, 450.0f);
    ecsManager_.addComponent<ecs::RigidBody>(ernestEntity_, 50.0f, 50.0f, false, true, 1.0f, 500.0f);
    ecsManager_.addComponent<ecs::Shape>(ernestEntity_, ecs::ShapeType::Rectangle, 50.0f, 50.0f, 0.0f, true);
    ecsManager_.addComponent<ecs::Color>(ernestEntity_, (Uint8)255, (Uint8)255, (Uint8)255, (Uint8)255);
    ecsManager_.addComponent<ecs::Velocity>(ernestEntity_, 0.0f, 0.0f, 400.0f, 500.0f);
    ecsManager_.addComponent<ecs::CollisionState>(ernestEntity_);

}

void scene::Demo::onUpdate(float deltaTime) {
    ecs::Color& bgColorComponent = ecsManager_.getComponent<ecs::Color>(bgEntity_);
    bgColorComponent.r = static_cast<Uint8>((std::sin(SDL_GetTicks() / 1000.0f) + 1) / 2 * 255);
    bgColorComponent.g = static_cast<Uint8>((std::sin(SDL_GetTicks() / 1000.0f + 2) + 1) / 2 * 255);
    bgColorComponent.b = static_cast<Uint8>((std::sin(SDL_GetTicks() / 1000.0f + 4) + 1) / 2 * 255);
}

void scene::Demo::onExit() {
    ecsManager_.clear();
}

void scene::Demo::handleEvents(const SDL_Event& event) {
}

void scene::Demo::handleInput() {
    const bool* keyboardState = SDL_GetKeyboardState(nullptr);

    ecs::Velocity& ernestVel = ecsManager_.getComponent<ecs::Velocity>(ernestEntity_);
    ecs::CollisionState& ernestCollision = ecsManager_.getComponent<ecs::CollisionState>(ernestEntity_);

    float stepSpeed = 100.0f; 
    float jumpSpeed = 300.0f;

    if (keyboardState[SDL_SCANCODE_A]) {
        ernestVel.x -= stepSpeed;
    }
    if (keyboardState[SDL_SCANCODE_D]) {
        ernestVel.x += stepSpeed;
    }
    // if is in collision of floor or collission of wall we can jumps
    if (keyboardState[SDL_SCANCODE_SPACE] && can_jump_()) {
        ernestVel.y -= jumpSpeed;
    }
    // if (keyboardState[SDL_SCANCODE_DOWN]) {
    //     ernestVel.y += stepSpeed;
    // }
}

bool scene::Demo::can_jump_() {

    ecs::CollisionState& collisionState = ecsManager_.getComponent<ecs::CollisionState>(ernestEntity_);
    ecs::Velocity& velocity = ecsManager_.getComponent<ecs::Velocity>(ernestEntity_);

    // if on ground it can jump
    if (collisionState.onGround) {
        return true;
    }

    // if on wall and falling we can wall jump
    if ((collisionState.onLeftWall || collisionState.onRightWall) && velocity.y >= -5) {
        return true;
    }

    return false;
}