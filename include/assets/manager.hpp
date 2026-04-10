#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

namespace assets {

class Manager {
public:
    Manager();
    ~Manager(); 

    SDL_Texture* loadTexture(const std::string& filePath, SDL_Renderer* renderer);

private:
    std::unordered_map<std::string, SDL_Texture*> textures_; // map of texture paths to SDL_Texture pointers
};

} // namespace assets