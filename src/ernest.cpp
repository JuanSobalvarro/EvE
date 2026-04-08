#include "ernest.hpp"

uwunya::Ernest::Ernest() {
    color = { 255, 255, 255, 255 };
    rect = { 0.0f, 0.0f, 1.0f, 1.0f };

    head = { 0.0f, 0.0f, 1.0f, 1.0f };
    body = { 0.0f, 0.0f, 1.0f, 1.0f };
    left_arm = { 0.0f, 0.0f, 1.0f, 1.0f };
    right_arm = { 0.0f, 0.0f, 1.0f, 1.0f };
    left_leg = { 0.0f, 0.0f, 1.0f, 1.0f };
    right_leg = { 0.0f, 0.0f, 1.0f, 1.0f };
}

uwunya::Ernest::~Ernest() {
}

SDL_FRect uwunya::Ernest::get_rect() const {
    return rect;
}

void uwunya::Ernest::draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_FRect body = {
        rect.x,
        rect.y,
        rect.w,
        rect.h
    };

    SDL_FRect head = {
        rect.x + rect.w * 0.25f,
        rect.y - rect.h * 0.5f,
        rect.w * 0.5f,
        rect.h * 0.5f
    };

    SDL_FRect left_arm = {
        rect.x - rect.w * 0.3f,
        rect.y + rect.h * 0.2f,
        rect.w * 0.3f,
        rect.h * 0.6f
    };

    SDL_FRect right_arm = {
        rect.x + rect.w,
        rect.y + rect.h * 0.2f,
        rect.w * 0.3f,
        rect.h * 0.6f
    };

    SDL_FRect left_leg = {
        rect.x + rect.w * 0.2f,
        rect.y + rect.h,
        rect.w * 0.2f,
        rect.h * 0.5f
    };

    SDL_FRect right_leg = {
        rect.x + rect.w * 0.6f,
        rect.y + rect.h,
        rect.w * 0.2f,
        rect.h * 0.5f
    };

    SDL_RenderFillRect(renderer, &body);
    SDL_RenderFillRect(renderer, &head);
    SDL_RenderFillRect(renderer, &left_arm);
    SDL_RenderFillRect(renderer, &right_arm);
    SDL_RenderFillRect(renderer, &left_leg);
    SDL_RenderFillRect(renderer, &right_leg);
}