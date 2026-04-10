#include "renderer/renderer.hpp"
#include <stdexcept>

renderer::Renderer::Renderer(const char* title, int width, int height) {
    window_ = SDL_CreateWindow(title, width, height, 0);
    if (!window_) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        throw std::runtime_error("Failed to create SDL window");
    }

    renderer_ = SDL_CreateRenderer(window_, nullptr);
    if (!renderer_) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window_);
        throw std::runtime_error("Failed to create SDL renderer");
    }
}

renderer::Renderer::~Renderer() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
}

SDL_Window* renderer::Renderer::getWindow() const {
    return window_;
}

SDL_Renderer* renderer::Renderer::getRenderer() const {
    return renderer_;
}

void renderer::Renderer::update(ecs::Manager& manager, float deltaTime) {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    std::vector<ecs::EntityId> entities = manager.getActiveEntities();

    // sort entities based on zIndex for correct rendering order
    std::sort(entities.begin(), entities.end(), [&manager](ecs::EntityId a, ecs::EntityId b) {
        int zA = manager.hasComponent<ecs::Transform>(a) ? manager.getComponent<ecs::Transform>(a).zIndex : 0;
        int zB = manager.hasComponent<ecs::Transform>(b) ? manager.getComponent<ecs::Transform>(b).zIndex : 0;

        if (zA == zB) {
            return a < b; // if same zIndex, sort by entity ID to ensure consistent order
        }
        
        return zA < zB;
    });

    ecs::Camera activeCamera;
    bool hasCamera = false;
    for (auto id : entities) {
        if (manager.hasComponent<ecs::Camera>(id)) {
            activeCamera = manager.getComponent<ecs::Camera>(id);
            hasCamera = true;
            break; 
        }
    }

    // Default to origin (0,0) and a standard 800x600 screen if no camera exists yet
    float camX = hasCamera ? activeCamera.x : 0.0f;
    float camY = hasCamera ? activeCamera.y : 0.0f;
    float halfScreenWidth = hasCamera ? (activeCamera.screenWidth / 2.0f) : 400.0f;
    float halfScreenHeight = hasCamera ? (activeCamera.screenHeight / 2.0f) : 300.0f;

    for (auto id : entities) {
        if (!manager.hasComponent<ecs::Transform>(id)) continue;

        const ecs::Transform& worldTransform = manager.getComponent<ecs::Transform>(id);
        
        ecs::Transform screenTransform = worldTransform;
        
        // For HUD elements, we ignore camera position and zoom, so they stay fixed on the screen
        if (manager.hasComponent<ecs::HUD>(id)) {
            screenTransform.x = worldTransform.x;
            screenTransform.y = worldTransform.y;
        } else {
            screenTransform.x = (worldTransform.x - camX) + halfScreenWidth;
            screenTransform.y = (worldTransform.y - camY) + halfScreenHeight;
        }
    
        if (manager.hasComponent<ecs::Color>(id) && manager.hasComponent<ecs::Shape>(id)) {
            const ecs::Color& color = manager.getComponent<ecs::Color>(id);
            const ecs::Shape& shape = manager.getComponent<ecs::Shape>(id);

            SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
            drawShape(screenTransform, shape);
        }

        if (manager.hasComponent<ecs::Sprite>(id)) {
            drawSprite(screenTransform, manager.getComponent<ecs::Sprite>(id));
        }

        if (manager.hasComponent<ecs::Geometry>(id)) {
            drawGeometry(screenTransform, manager.getComponent<ecs::Geometry>(id));
        }
    }
    SDL_RenderPresent(renderer_);
}

void renderer::Renderer::drawShape(const ecs::Transform& transform, const ecs::Shape& shape) {
    if (shape.type == ecs::ShapeType::Rectangle) {
        SDL_FRect rect;
        rect.x = transform.x;
        rect.y = transform.y;
        rect.w = shape.width * transform.scaleX;
        rect.h = shape.height * transform.scaleY;

        if (shape.filled) {
            SDL_RenderFillRect(renderer_, &rect);
        } else {
            SDL_RenderRect(renderer_, &rect);
        }
    } else if (shape.type == ecs::ShapeType::Circle) {
        throw std::runtime_error("Circle shape type not implemented yet");
    }
}

void renderer::Renderer::drawSprite(const ecs::Transform& transform, const ecs::Sprite& sprite) {
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

    Uint8 alpha = sprite.alpha;
    SDL_SetTextureAlphaMod(sprite.texture, alpha);

    float texW = 0.0f;
    float texH = 0.0f;
    SDL_GetTextureSize(sprite.texture, &texW, &texH);

    SDL_FRect destRect;
    destRect.x = transform.x;
    destRect.y = transform.y;
    destRect.w = texW * transform.scaleX;
    destRect.h = texH * transform.scaleY;

    if (transform.rotation != 0.0f) {
        SDL_RenderTextureRotated(renderer_, sprite.texture, nullptr, &destRect, transform.rotation, nullptr, SDL_FLIP_NONE);
    } else {
        SDL_RenderTexture(renderer_, sprite.texture, nullptr, &destRect);
        // std::cout << "Drawing sprite at (" << destRect.x << ", " << destRect.y << ") with size (" << destRect.w << ", " << destRect.h << ")\n";
    }
}

void renderer::Renderer::drawGeometry(const ecs::Transform& transform, const ecs::Geometry& geometry) {
    if (geometry.vertices.empty()) return;

    static std::vector<SDL_Vertex> transformedVertices;
    transformedVertices.resize(geometry.vertices.size());

    for (size_t i = 0; i < geometry.vertices.size(); ++i) {
        transformedVertices[i] = geometry.vertices[i];
        
        transformedVertices[i].position.x *= transform.scaleX;
        transformedVertices[i].position.y *= transform.scaleY;

        transformedVertices[i].position.x += transform.x;
        transformedVertices[i].position.y += transform.y;
    }

    const int* indices = geometry.indices.empty() ? nullptr : geometry.indices.data();
    int numIndices = geometry.indices.empty() ? 0 : static_cast<int>(geometry.indices.size());

    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);

    SDL_RenderGeometry(
        renderer_, 
        geometry.texture, 
        transformedVertices.data(), 
        static_cast<int>(transformedVertices.size()), 
        indices, 
        numIndices
    );
}