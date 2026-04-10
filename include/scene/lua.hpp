#pragma once

#define SOL_ALL_SAFETIES_ON 1

#include <sol/sol.hpp>
#include <filesystem>

#include "scene/scene.hpp"
#include "ecs/components.hpp"

namespace core { class Game; } // forward declaration to avoid circular dependency

namespace scene {

class LuaScene : public Scene {
public:
    LuaScene(ecs::Manager& ecsManager, assets::Manager& assetManager, ecs::PhysicsSystem& physicsSystem, renderer::Renderer& rendererSystem, core::Game& game, const std::string& scriptPath);
    ~LuaScene() override;
    void onEnter() override;
    void onUpdate(float deltaTime) override;
    void onExit() override;
    void handleEvents(const SDL_Event& event) override;
    void handleInput() override;

private:
    void bindECS();
    void loadAndExecute();

    sol::state lua_;
    std::string scriptPath_;
    std::filesystem::file_time_type lastWriteTime_;
};

}