#pragma once

#include "entity.hpp"

namespace uwunya {

class Floor : public Entity {
public:
    Floor();
    ~Floor();

    void draw(SDL_Renderer* renderer) override;
private:
    SDL_Color color;
};

}