#pragma once

#include <SDL3/SDL.h>

#include "ecs/manager.hpp"
#include "ecs/components.hpp"

namespace renderer {

class Renderer {
public:

    void update(ecs::Manager& manager, SDL_Renderer* renderer, float deltaTime);
};


} // namespace renderer