#pragma once

#include <SDL3/SDL.h>

namespace ecs {

struct Transform {
    float x = 0.0f;
    float y = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float rotation = 0.0f; 
};

struct Velocity {
    float x = 0.0f;
    float y = 0.0f;
    float maxSpeed_x = 0.0f;
    float maxSpeed_y = 0.0f;
};

struct RigidBody {
    float width = 0.0f;
    float height = 0.0f;
    bool isStatic = true;
    bool collidable = true;
    float friction = 1.0f;
    float gravity = 0.5f;
};

struct Sprite {
    SDL_Texture* texture = nullptr;
};

struct Color {
    Uint8 r = 0;
    Uint8 g = 0;
    Uint8 b = 0;
    Uint8 a = 255;
};

enum class ShapeType {
    Rectangle,
    Circle
};

struct Shape {
    ShapeType type = ShapeType::Rectangle;
    float width = 0.0f;
    float height = 0.0f;
    float radius = 0.0f;
    bool filled = true;
};

struct CollisionState {
    bool onGround = false;
    bool onLeftWall = false;
    bool onRightWall = false;
    bool onCeiling = false;
};

}