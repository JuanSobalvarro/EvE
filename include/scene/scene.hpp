#pragma once

#include <SDL3/SDL.h>

#include "ecs/manager.hpp"
#include "assets/manager.hpp"

namespace core { class Game; } // forward declaration to avoid circular dependency

namespace scene {

class Scene {
public:
    Scene(ecs::Manager& ecsManager, assets::Manager& assetManager, core::Game& game) : ecsManager_(ecsManager), assetManager_(assetManager), game_(game) {};
    virtual ~Scene() = default;

    virtual void onEnter() = 0;
    virtual void onUpdate(float deltaTime) = 0;
    virtual void onExit() = 0;
    virtual void handleEvents(const SDL_Event& event) = 0; // optional to override if the scene needs to handle events, by default do nothing
    virtual void handleInput() = 0; // optional where we consult mouse, keyboards etc states
protected:
    ecs::Manager& ecsManager_;
    assets::Manager& assetManager_;
    core::Game& game_;
};

}