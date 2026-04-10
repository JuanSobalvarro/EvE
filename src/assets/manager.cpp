#include "assets/manager.hpp"

assets::Manager::Manager() {
    // nothing to initialize in the constructor since we will load textures on demand
}

assets::Manager::~Manager() {
    // free all loaded textures
    for (auto& pair : textures_) {
        if (pair.second != nullptr) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textures_.clear();
}

SDL_Texture* assets::Manager::loadTexture(const std::string& filePath, SDL_Renderer* renderer) {
    if (renderer == nullptr) {
        std::cerr << "Error: Renderer is null in loadTexture for file: " << filePath << std::endl;
        return nullptr;
    }
    
    // Check Cache
    auto it = textures_.find(filePath);
    if (it != textures_.end()) {
        return it->second;
    }

    // Resolve Path
    // If the path starts with "./", we can strip it or just trust SDL_image.
    // Most OS's handle "./" fine, but "assets/..." is cleaner.
    
    // Load from disk
    SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());
    if (!texture) {\
        // const char* base = SDL_GetBasePath();
        // if (!base) {
        //     std::cerr << "SDL_GetBasePath Error: " << SDL_GetError() << std::endl;
        //     return nullptr;
        // }
        // std::cerr << "IMG_Error: " << SDL_GetError() << std::endl;
        // std::cerr << "CWD: " << (base ? base : "Unknown") << std::endl;
        // std::cerr << "Attempted path: " << filePath << std::endl;
        // SDL_free((void*)base);
        return nullptr;
    }

    textures_[filePath] = texture;
    return texture;
}