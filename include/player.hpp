#pragma once

#include "entity.hpp"

namespace uwunya {

class Player : public Entity {
public:
    Player();
    ~Player();

    void draw(SDL_Renderer* renderer) override;

    SDL_FRect get_rect() const;
private:
    SDL_Color color;

    SDL_FRect head;
    SDL_FRect body;
    SDL_FRect left_arm;
    SDL_FRect right_arm;
    SDL_FRect left_leg;
    SDL_FRect right_leg;
};



}