#include "renderer/renderer.hpp"

void renderer::Renderer::update(ecs::Manager& manager, SDL_Renderer* renderer, float deltaTime) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (auto id : manager.getActiveEntities()) {
        if (!manager.hasComponent<ecs::Transform>(id)) continue;
        auto& trans = manager.getComponent<ecs::Transform>(id);

        if (manager.hasComponent<ecs::Rectangle>(id)) {
            auto& rect = manager.getComponent<ecs::Rectangle>(id);
            
            // apply color if it exists
            if (manager.hasComponent<ecs::Color>(id)) {
                auto& c = manager.getComponent<ecs::Color>(id);
                SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }

            SDL_FRect drawRect = { trans.x, trans.y, rect.width * trans.scaleX, rect.height * trans.scaleY };
            
            if (rect.filled) SDL_RenderFillRect(renderer, &drawRect);
            else SDL_RenderRect(renderer, &drawRect);
        }

        else if (manager.hasComponent<ecs::Sprite>(id)) {
            auto& sprite = manager.getComponent<ecs::Sprite>(id);
            // TODO: Handle sprite rendering with rotation and scaling
        }
    }
    SDL_RenderPresent(renderer);
}