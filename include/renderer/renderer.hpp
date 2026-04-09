#pragma once

#include <SDL3/SDL.h>

#include "ecs/manager.hpp"
#include "ecs/components.hpp"

namespace renderer {

class Renderer {
public:

    void update(ecs::Manager& manager, SDL_Renderer* renderer, float deltaTime);

private:

    void drawShape(SDL_Renderer* renderer, const ecs::Transform& transform, const ecs::Shape& shape);
    void drawSprite(SDL_Renderer* renderer, const ecs::Transform& transform, const ecs::Sprite& sprite);
    void drawGeometry(SDL_Renderer* renderer, const ecs::Transform& transform, const ecs::Geometry& geometry);

};

} // namespace renderer