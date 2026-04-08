#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include <vector>
#include <cmath>

#include "ecs/manager.hpp"
#include "assets/manager.hpp"
#include "renderer/renderer.hpp"

namespace core {

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int width, int height);
    
    void run();
    
    void stop();

private:
    void handleEvents();
    void update(float deltaTime);
    void render(float deltaTime);

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    bool isRunning_ = false;

    std::unique_ptr<ecs::Manager> ecsManager_;
    std::unique_ptr<assets::Manager> assetManager_;
    std::unique_ptr<renderer::Renderer> rendererSystem_;

    void setupDemo(); 
};

} // namespace core