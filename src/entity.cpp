#include "entity.hpp"

void uwunya::Entity::draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void uwunya::Entity::move(float x, float y) {
    rect.x += x;
    rect.y += y;
}

void uwunya::Entity::resize(float w, float h) {
    rect.w = w;
    rect.h = h;
}
