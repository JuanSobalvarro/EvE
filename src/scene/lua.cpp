#include "scene/lua.hpp"
#include <iostream>

namespace scene {

LuaScene::LuaScene(ecs::Manager& ecsManager, assets::Manager& assetManager, ecs::PhysicsSystem& physicsSystem, renderer::Renderer& rendererSystem, core::Game& game, const std::string& scriptPath) 
    : Scene(ecsManager, assetManager, physicsSystem, rendererSystem, game), scriptPath_(scriptPath) {
    
    lua_.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table, sol::lib::os);
    bindECS();
}

LuaScene::~LuaScene() {
}

void LuaScene::bindECS() {
    lua_.new_enum("ShapeType",
        "Rectangle", ecs::ShapeType::Rectangle,
        "Circle", ecs::ShapeType::Circle
    );

    lua_.new_enum("BlendType",
        "Normal", ecs::BlendType::Normal,
        "Additive", ecs::BlendType::Additive,
        "Multiply", ecs::BlendType::Multiply
    );

    lua_.new_usertype<ecs::Transform>("Transform",
        "x", &ecs::Transform::x,
        "y", &ecs::Transform::y,
        "scaleX", &ecs::Transform::scaleX,
        "scaleY", &ecs::Transform::scaleY,
        "rotation", &ecs::Transform::rotation
    );

    lua_.new_usertype<ecs::Velocity>("Velocity",
        "x", &ecs::Velocity::x,
        "y", &ecs::Velocity::y,
        "maxSpeed_x", &ecs::Velocity::maxSpeed_x,
        "maxSpeed_y", &ecs::Velocity::maxSpeed_y
    );

    lua_.new_usertype<ecs::RigidBody>("RigidBody",
        "width", &ecs::RigidBody::width,
        "height", &ecs::RigidBody::height,
        "isStatic", &ecs::RigidBody::isStatic,
        "collidable", &ecs::RigidBody::collidable,
        "friction", &ecs::RigidBody::friction,
        "gravity", &ecs::RigidBody::gravity
    );

    lua_.new_usertype<ecs::Sprite>("Sprite",
        "texture", &ecs::Sprite::texture,
        "blend", &ecs::Sprite::blend,
        "alpha", &ecs::Sprite::alpha
    );

    lua_.new_usertype<ecs::Color>("Color",
        "r", &ecs::Color::r,
        "g", &ecs::Color::g,
        "b", &ecs::Color::b,
        "a", &ecs::Color::a
    );

    lua_.new_usertype<ecs::Shape>("Shape",
        "type", &ecs::Shape::type,
        "width", &ecs::Shape::width,
        "height", &ecs::Shape::height,
        "radius", &ecs::Shape::radius,
        "filled", &ecs::Shape::filled
    );

    lua_.new_usertype<ecs::CollisionState>("CollisionState",
        "onGround", &ecs::CollisionState::onGround,
        "onLeftWall", &ecs::CollisionState::onLeftWall,
        "onRightWall", &ecs::CollisionState::onRightWall,
        "onCeiling", &ecs::CollisionState::onCeiling
    );

    lua_.new_usertype<ecs::Geometry>("Geometry",
        "vertices", &ecs::Geometry::vertices,
        "indices", &ecs::Geometry::indices,
        "texture", &ecs::Geometry::texture
    );
    lua_.new_enum("TweenProperty",
        "PositionX", ecs::TweenProperty::PositionX,
        "PositionY", ecs::TweenProperty::PositionY,
        "ScaleX", ecs::TweenProperty::ScaleX,
        "ScaleY", ecs::TweenProperty::ScaleY,
        "Rotation", ecs::TweenProperty::Rotation,

        "ColorR", ecs::TweenProperty::ColorR,
        "ColorG", ecs::TweenProperty::ColorG,
        "ColorB", ecs::TweenProperty::ColorB,
        "ColorA", ecs::TweenProperty::ColorA,

        "SpriteAlpha", ecs::TweenProperty::SpriteAlpha
    );
    lua_.new_enum("EaseType",
        "Linear", ecs::EaseType::Linear,
        "InQuad", ecs::EaseType::InQuad,
        "OutQuad", ecs::EaseType::OutQuad,
        "InOutQuad", ecs::EaseType::InOutQuad,
        "InCubic", ecs::EaseType::InCubic,
        "OutCubic", ecs::EaseType::OutCubic,
        "InOutCubic", ecs::EaseType::InOutCubic
    );

    lua_.set_function("createEntity", [&]() { return ecsManager_.createEntity(); });
    lua_.set_function("destroyEntity", [&](ecs::EntityId id) { ecsManager_.destroyEntity(id); });
    lua_.set_function("addTransform", [&](ecs::EntityId id, float x, float y, float sx, float sy, float r) { ecsManager_.addComponent<ecs::Transform>(id, x, y, sx, sy, r); });
    lua_.set_function("addVelocity", [&](ecs::EntityId id, float x, float y, float mx, float my) { ecsManager_.addComponent<ecs::Velocity>(id, x, y, mx, my); });
    lua_.set_function("addRigidBody", [&](ecs::EntityId id, float w, float h, bool isStatic, bool collidable, float friction, float gravity, float mass, float bounce) { ecsManager_.addComponent<ecs::RigidBody>(id, w, h, isStatic, collidable, friction, gravity, mass, bounce); });
    lua_.set_function("addSprite", [&](ecs::EntityId id, const std::string& path, ecs::BlendType blend, sol::optional<Uint8> alpha) {
        SDL_Texture* tex = assetManager_.loadTexture(path, rendererSystem_.getRenderer());
        
        if (tex) {
            ecsManager_.addComponent<ecs::Sprite>(id, tex, blend, alpha.value_or(255));
        } else {
            std::cerr << "Failed to load sprite: " << path << std::endl;
        }
    });
    lua_.set_function("addColor", [&](ecs::EntityId id, Uint8 r, Uint8 g, Uint8 b, Uint8 a) { ecsManager_.addComponent<ecs::Color>(id, r, g, b, a); });
    lua_.set_function("addShape", [&](ecs::EntityId id, ecs::ShapeType t, float w, float h, float r, bool f) { ecsManager_.addComponent<ecs::Shape>(id, t, w, h, r, f); });
    lua_.set_function("addCollisionState", [&](ecs::EntityId id) { ecsManager_.addComponent<ecs::CollisionState>(id); });

    lua_.set_function("addGeometryQuad", [&](ecs::EntityId id, float width, float height) {
        std::vector<SDL_Vertex> vertices;
        std::vector<int> indices;

        vertices.push_back({{0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}});
        vertices.push_back({{width, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}});
        vertices.push_back({{0.0f, height}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}});
        vertices.push_back({{width, height}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}});

        indices = {0, 1, 2, 1, 3, 2};

        ecsManager_.addComponent<ecs::Geometry>(id, vertices, indices, nullptr);
    });

    lua_.set_function("addGeometryOrb", [&](ecs::EntityId id, float radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
        std::vector<SDL_Vertex> vertices;
        std::vector<int> indices;

        int segments = 32; // How smooth the circle is
        
        // center vertex
        vertices.push_back({{radius, radius}, {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f}, {0.5f, 0.5f}});

        // Outer edge vertices (Zero opacity for the soft fade)
        for (int i = 0; i < segments; ++i) {
            float angle = (i * 2.0f * M_PI) / segments;
            float x = radius + std::cos(angle) * radius;
            float y = radius + std::sin(angle) * radius;
            
            // alpha is 0 
            vertices.push_back({{x, y}, {r / 255.0f, g / 255.0f, b / 255.0f, 0.0f}, {0.5f, 0.5f}});
        }

        // Connect the dots using triangle indices
        for (int i = 1; i <= segments; ++i) {
            indices.push_back(0); // Center point
            indices.push_back(i); // Current edge point
            indices.push_back((i % segments) + 1); // Next edge point (wraps around to 1 at the end)
        }

        ecsManager_.addComponent<ecs::Geometry>(id, vertices, indices, nullptr);
    });

    lua_.set_function("addGeometryCustom", [&](ecs::EntityId id, sol::table luaVertices, sol::table luaIndices) {
        std::vector<SDL_Vertex> vertices;
        std::vector<int> indices;

        for (size_t i = 1; i <= luaVertices.size(); ++i) {
            sol::table v = luaVertices[i];
            SDL_Vertex vertex;
            
            vertex.position.x = v.get_or("x", 0.0f);
            vertex.position.y = v.get_or("y", 0.0f);
            
            vertex.color.r = v.get_or("r", 1.0f);
            vertex.color.g = v.get_or("g", 1.0f);
            vertex.color.b = v.get_or("b", 1.0f);
            vertex.color.a = v.get_or("a", 1.0f);
            
            vertex.tex_coord.x = v.get_or("u", 0.0f);
            vertex.tex_coord.y = v.get_or("v", 0.0f);
            
            vertices.push_back(vertex);
        }

        for (size_t i = 1; i <= luaIndices.size(); ++i) {
            indices.push_back(luaIndices.get<int>(i));
        }

        ecsManager_.addComponent<ecs::Geometry>(id, vertices, indices, nullptr);
    });
    lua_.set_function("addCamera", [&](ecs::EntityId id, float x, float y, float screenWidth, float screenHeight, float zoom) {
        ecsManager_.addComponent<ecs::Camera>(id, x, y, screenWidth, screenHeight, zoom);
    });

    lua_.set_function("getTransform", [&](ecs::EntityId id) -> ecs::Transform& { return ecsManager_.getComponent<ecs::Transform>(id); });
    lua_.set_function("getVelocity", [&](ecs::EntityId id) -> ecs::Velocity& { return ecsManager_.getComponent<ecs::Velocity>(id); });
    lua_.set_function("getRigidBody", [&](ecs::EntityId id) -> ecs::RigidBody& { return ecsManager_.getComponent<ecs::RigidBody>(id); });
    lua_.set_function("getSprite", [&](ecs::EntityId id) -> ecs::Sprite& { return ecsManager_.getComponent<ecs::Sprite>(id); });
    lua_.set_function("getColor", [&](ecs::EntityId id) -> ecs::Color& { return ecsManager_.getComponent<ecs::Color>(id); });
    lua_.set_function("getShape", [&](ecs::EntityId id) -> ecs::Shape& { return ecsManager_.getComponent<ecs::Shape>(id); });
    lua_.set_function("getCollisionState", [&](ecs::EntityId id) -> ecs::CollisionState& { return ecsManager_.getComponent<ecs::CollisionState>(id); });
    lua_.set_function("getGeometry", [&](ecs::EntityId id) -> ecs::Geometry& { return ecsManager_.getComponent<ecs::Geometry>(id); });
    
    lua_.set_function("isKeyDown", [&](int scancode) {
        const bool* state = SDL_GetKeyboardState(nullptr);
        return state[scancode];
    });

    lua_.set_function("isMouseButtonDown", [&](int button) {
        SDL_MouseButtonFlags buttons = SDL_GetMouseState(nullptr, nullptr);
        return (buttons & SDL_BUTTON_MASK(button)) != 0;
    });

    lua_.set_function("getMousePosition", [&]() {
        float x, y;
        SDL_GetMouseState(&x, &y);
        return std::make_tuple(x, y);
    });

    lua_.set_function("clearScene", [&]() { ecsManager_.clear(); });

    lua_.set_function("getEntityCount", [&]() { return ecsManager_.getActiveEntities().size(); });

    lua_.set_function("setWorld", [&](int width, int height, int cellSize) { physicsSystem_.setWorld(width, height, cellSize); });
}

void LuaScene::loadAndExecute() {
    sol::load_result script = lua_.load_file(scriptPath_);
    if (!script.valid()) {
        sol::error err = script;
        std::cerr << err.what() << std::endl;
        return;
    }
    
    sol::protected_function_result result = script();
    if (!result.valid()) {
        sol::error err = result;
        std::cerr << err.what() << std::endl;
        return;
    }

    sol::protected_function enterFunc = lua_["on_enter"];
    if (enterFunc.valid()) {
        sol::protected_function_result enterResult = enterFunc();
        if (!enterResult.valid()) {
            sol::error err = enterResult;
            std::cerr << err.what() << std::endl;
        }
    }
}

void LuaScene::onEnter() {
    std::error_code ec;
    lastWriteTime_ = std::filesystem::last_write_time(scriptPath_, ec);
    loadAndExecute();
}

void LuaScene::onUpdate(float deltaTime) {
    std::error_code ec;
    auto currentTime = std::filesystem::last_write_time(scriptPath_, ec);

    if (!ec && currentTime > lastWriteTime_) {
        lastWriteTime_ = currentTime;
        std::cout << "Reloading script: " << scriptPath_ << std::endl;
        ecsManager_.clear();
        loadAndExecute();
    }

    sol::protected_function updateFunc = lua_["update"];
    if (updateFunc.valid()) {
        sol::protected_function_result result = updateFunc(deltaTime);
        if (!result.valid()) {
            sol::error err = result;
            std::cerr << err.what() << std::endl;
        }
    }
}

void LuaScene::onExit() {
    sol::protected_function exitFunc = lua_["exit"];
    if (exitFunc.valid()) {
        sol::protected_function_result result = exitFunc();
        if (!result.valid()) {
            sol::error err = result;
            std::cerr << err.what() << std::endl;
        }
    }
    ecsManager_.clear();
}

void LuaScene::handleEvents(const SDL_Event& event) {
}

void LuaScene::handleInput() {
    sol::protected_function handleInputFunc = lua_["handle_input"];
    if (handleInputFunc.valid()) {
        sol::protected_function_result result = handleInputFunc();
        if (!result.valid()) {
            sol::error err = result;
            std::cerr << err.what() << std::endl;
        }
    }
}

}