#include "renderer/renderer.hpp"

void renderer::Renderer::update(ecs::Manager& manager, SDL_Renderer* renderer, float deltaTime) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (auto id : manager.getActiveEntities()) {
        // if there is no transform component we are not rendering anything for this entity, so we skip it
        if (!manager.hasComponent<ecs::Transform>(id)) continue;

        const ecs::Transform& transform = manager.getComponent<ecs::Transform>(id);
    
        // render shapes that have a color and shape component (debug boxes, primitive geometry, etc)
        if (manager.hasComponent<ecs::Color>(id) && manager.hasComponent<ecs::Shape>(id)) {
            const ecs::Color& color = manager.getComponent<ecs::Color>(id);
            const ecs::Shape& shape = manager.getComponent<ecs::Shape>(id);

            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

            drawShape(renderer, transform, shape);
        }

        if (manager.hasComponent<ecs::Sprite>(id)) {
            drawSprite(renderer, transform, manager.getComponent<ecs::Sprite>(id));
        }

        if (manager.hasComponent<ecs::Geometry>(id)) {
            drawGeometry(renderer, transform, manager.getComponent<ecs::Geometry>(id));
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

void renderer::Renderer::drawSprite(SDL_Renderer* renderer, const ecs::Transform& transform, const ecs::Sprite& sprite) {
    if (sprite.texture == nullptr) {
        throw std::runtime_error("Sprite component has null texture");
    }

    switch (sprite.blend) {
        case ecs::BlendType::Normal:
            SDL_SetTextureBlendMode(sprite.texture, SDL_BLENDMODE_BLEND);
            break;
        case ecs::BlendType::Additive:
            SDL_SetTextureBlendMode(sprite.texture, SDL_BLENDMODE_ADD);
            break;
        case ecs::BlendType::Multiply:
            SDL_SetTextureBlendMode(sprite.texture, SDL_BLENDMODE_MOD);
            break;
    }

    float texW = 0.0f;
    float texH = 0.0f;
    SDL_GetTextureSize(sprite.texture, &texW, &texH);

    SDL_FRect destRect;
    destRect.x = transform.x;
    destRect.y = transform.y;
    destRect.w = texW * transform.scaleX;
    destRect.h = texH * transform.scaleY;

    if (transform.rotation != 0.0f) {
        SDL_RenderTextureRotated(renderer, sprite.texture, nullptr, &destRect, transform.rotation, nullptr, SDL_FLIP_NONE);
    } else {
        SDL_RenderTexture(renderer, sprite.texture, nullptr, &destRect);
    }
}

void renderer::Renderer::drawGeometry(SDL_Renderer* renderer, const ecs::Transform& transform, const ecs::Geometry& geometry) {
    if (geometry.vertices.empty()) return;

    static std::vector<SDL_Vertex> transformedVertices;
    transformedVertices.resize(geometry.vertices.size());

    for (size_t i = 0; i < geometry.vertices.size(); ++i) {
        transformedVertices[i] = geometry.vertices[i];
        
        // Apply Scale
        transformedVertices[i].position.x *= transform.scaleX;
        transformedVertices[i].position.y *= transform.scaleY;

        // Apply Position Offset
        transformedVertices[i].position.x += transform.x;
        transformedVertices[i].position.y += transform.y;
    }

    const int* indices = geometry.indices.empty() ? nullptr : geometry.indices.data();
    int numIndices = geometry.indices.empty() ? 0 : static_cast<int>(geometry.indices.size());

    SDL_RenderGeometry(
        renderer, 
        geometry.texture, // optional texture for textured geometry, can be nullptr for untextured
        transformedVertices.data(), 
        static_cast<int>(transformedVertices.size()), 
        indices, 
        numIndices
    );
}

