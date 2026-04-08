#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include <vector>
#include <cmath>

#include "ecs/manager.hpp"
#include "assets/manager.hpp"
#include "renderer/renderer.hpp"
#include "scene/scene.hpp"
#include "ecs/physics.hpp"

#include "scene/demo.hpp"

namespace core {

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

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    bool isRunning_ = false;

    std::unique_ptr<ecs::Manager> ecsManager_;
    std::unique_ptr<assets::Manager> assetManager_;
    std::unique_ptr<renderer::Renderer> rendererSystem_;
    std::unique_ptr<ecs::PhysicsSystem> physicsSystem_;

    std::unique_ptr<scene::Scene> currentScene_;
};

} // namespace core