#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include <vector>
#include <cmath>
#include <iostream>

#include "ecs/manager.hpp"
#include "assets/manager.hpp"
#include "renderer/renderer.hpp"
#include "scene/scene.hpp"
#include "ecs/physics.hpp"
#include "ecs/animation.hpp"
#include "ecs/audio.hpp"

#include "scene/lua.hpp"

namespace core {

struct GameSystems {
    std::unique_ptr<renderer::Renderer> renderer;
    std::unique_ptr<ecs::PhysicsSystem> physics;
    std::unique_ptr<ecs::AnimationSystem> animation;
    std::unique_ptr<ecs::AudioSystem> audio;
};

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int width, int height);
    void run();
    void stop();

    void changeScene(std::unique_ptr<scene::Scene> newScene);

private:
    void handleEvents();
    void handleInput(); // handle global input like quitting, pausing, etc. not scene specific input
    void update(float deltaTime);
    void render(float deltaTime);
    
    bool isRunning_ = false;

    std::unique_ptr<ecs::Manager> ecsManager_;
    std::unique_ptr<assets::Manager> assetManager_;
    
    GameSystems gameSystems_;

    std::unique_ptr<scene::Scene> currentScene_;
};

} // namespace core