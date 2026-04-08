#include "renderer/renderer.hpp"

void renderer::Renderer::update(ecs::Manager& manager, SDL_Renderer* renderer, float deltaTime) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (auto id : manager.getActiveEntities()) {
        // if there is no transform component we are not rendering anything for this entity, so we skip it
        if (!manager.hasComponent<ecs::Transform>(id)) continue;

        const ecs::Transform& transform = manager.getComponent<ecs::Transform>(id);
    
        // if it has color and shape then we need to render a shape
        if (manager.hasComponent<ecs::Color>(id) && manager.hasComponent<ecs::Shape>(id)) {
            const ecs::Color& color = manager.getComponent<ecs::Color>(id);
            const ecs::Shape& shape = manager.getComponent<ecs::Shape>(id);

            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

            drawShape(renderer, transform, shape);
        }
    }
    SDL_RenderPresent(renderer);
}

void renderer::Renderer::drawShape(SDL_Renderer* renderer, const ecs::Transform& transform, const ecs::Shape& shape) {
    if (shape.type == ecs::ShapeType::Rectangle) {
        SDL_FRect rect;
        rect.x = transform.x;
        rect.y = transform.y;
        rect.w = shape.width * transform.scaleX;
        rect.h = shape.height * transform.scaleY;

        if (shape.filled) {
            SDL_RenderFillRect(renderer, &rect);
        } else {
            SDL_RenderRect(renderer, &rect);
        }
    } else if (shape.type == ecs::ShapeType::Circle) {
        throw std::runtime_error("Circle shape type not implemented yet");
    }
}