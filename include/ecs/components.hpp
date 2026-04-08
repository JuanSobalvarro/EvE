#pragma once

namespace ecs {

class Transform {
public:
    Transform();
    ~Transform();

private:
    float x_;
    float y_;
    float z_;
};


class Velocity {
public:
    Velocity();
    ~Velocity();
private:
    float vx_;
    float vy_;
    float vz_;
};

class Sprite {
public:
    Sprite();
    ~Sprite();
private:
    int spriteId_;
};

}