#pragma once

#include <cstdint>

#include "ecs/manager.hpp"

namespace ecs {

class Entity {
public:
    Entity();
    ~Entity();

    uint64_t getId() const;
    void setId(uint64_t id);

private:
    uint64_t id_;
};

}