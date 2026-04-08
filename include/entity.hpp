#pragma once

#include <SDL3/SDL.h>

namespace uwunya {

class Entity {
public:
    virtual ~Entity() = default;

    virtual void move(float x, float y);
    virtual void resize(float w, float h);
    virtual void draw(SDL_Renderer* renderer);
protected:
    SDL_FRect rect;
};

//
} // namespace uwunya