#pragma once

#include <SDL3/SDL.h>
#include <iostream>

#include "ecs/manager.hpp"
#include "ecs/components.hpp"

namespace renderer {

class Renderer {
public:
    Renderer(const char* title, int width, int height);
    ~Renderer();

    SDL_Window* getWindow() const;
    SDL_Renderer* getRenderer() const;

    void update(ecs::Manager& manager, float deltaTime);

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;

    void drawShape(const ecs::Transform& transform, const ecs::Shape& shape);
    void drawSprite(const ecs::Transform& transform, const ecs::Sprite& sprite);
    void drawGeometry(const ecs::Transform& transform, const ecs::Geometry& geometry);

};

} // namespace renderer