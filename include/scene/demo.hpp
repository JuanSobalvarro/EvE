#pragma once

#include <SDL3/SDL.h>
#include <cmath>

#include "scene/scene.hpp"
#include "assets/manager.hpp"
#include "ecs/manager.hpp"
#include "ecs/components.hpp"
#include "ecs/types.hpp"

namespace scene {
class Demo : public Scene {
public:
    Demo(ecs::Manager& ecsManager, assets::Manager& assetManager, core::Game& game);
    ~Demo() override;
    void onEnter() override;
    void onUpdate(float deltaTime) override;
    void onExit() override;
    void handleEvents(const SDL_Event& event) override;
    void handleInput() override;
private:
    ecs::EntityId bgEntity_;
    ecs::EntityId floorEntity_;
    ecs::EntityId ernestEntity_;

    bool can_jump_();
};
}