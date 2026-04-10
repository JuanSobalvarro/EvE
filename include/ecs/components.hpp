#pragma once

#include <SDL3/SDL.h>
#include <vector>

#include "ecs/types.hpp"

namespace ecs {

struct Transform {
    float x = 0.0f;
    float y = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float rotation = 0.0f; 
    int zIndex = 0; // for rendering order
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
    float mass = 1.0f;
    float bounce = 0.5f;
};

enum class BlendType {
    Normal,
    Additive,
    Multiply
};

struct Sprite {
    SDL_Texture* texture = nullptr;
    BlendType blend = BlendType::Normal;
    Uint8 alpha = 255;
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

struct Geometry {
    std::vector<SDL_Vertex> vertices;
    std::vector<int> indices;
    SDL_Texture* texture = nullptr;
};

struct Camera {
    float x = 0.0f; // Center of the camera in World Space
    float y = 0.0f;
    float screenWidth = 800.0f; // Your SDL window width
    float screenHeight = 600.0f; // Your SDL window height
    float zoom = 1.0f;
};

// HUD tells the renderer that this entity corresponds to a UI element
struct HUD {
    bool interactive = false; // if true this entity will be considered for input events
};

enum class TweenProperty {
    // Transform properties
    PositionX,
    PositionY,
    ScaleX,
    ScaleY,
    Rotation,

    // Color properties
    ColorR,
    ColorG,
    ColorB,
    ColorA,

    // sprite alpha
    SpriteAlpha,
};

enum class EaseType {
    Linear,
    InQuad,
    OutQuad,
    InOutQuad,
    InCubic,
    OutCubic,
    InOutCubic
};

struct Animation {
    struct TweenData {
        TweenProperty property;
        float startValue;
        float endValue;
        float duration;
        float elapsedTime = 0.0f;
        float delay = 0.0f;
        EaseType easeType;
        bool active = true;
    };
    std::vector<TweenData> tweens;
};

}