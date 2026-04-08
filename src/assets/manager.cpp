#include "assets/manager.hpp"

assets::Manager::Manager(SDL_Renderer* renderer) : renderer_(renderer) {
    // nothing to initialize in the constructor since we will load textures on demand
}

assets::Manager::~Manager() {
    // free all loaded textures
    for (auto& pair : textures_) {
        SDL_DestroyTexture(pair.second);
    }
    textures_.clear();
}

SDL_Texture* assets::Manager::loadTexture(const std::string& filePath) {
    // if we have it just return the existing texture
    auto it = textures_.find(filePath);
    if (it != textures_.end()) {
        return it->second;
    }

    // load it from disk
    SDL_Texture* texture = IMG_LoadTexture(renderer_, filePath.c_str());
    if (!texture) {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
        return nullptr;
    }

    // store it in the map and return it
    textures_[filePath] = texture;
    return texture;
} 