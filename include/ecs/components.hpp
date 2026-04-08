#pragma once

#include <SDL3/SDL.h>

namespace ecs {

struct Transform {
    float x;
    float y;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float rotation = 0.0f; // in degrees
};

struct Sprite {
    SDL_Texture* textureId; 
    int width;     
    int height;    
    SDL_FRect srcRect; 
};

struct Color {
    Uint8 r = 255, g = 255, b = 255, a = 255;
};

struct Animation {
    int frameCount;     
    int currentFrame;  
    float frameTime;   
    float timer = 0.0f;
    bool loop = true;
};

struct Rectangle {
    float width;
    float height;
    bool filled = true;
};

//
} // namespace ecs