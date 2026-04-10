#include "core/game.hpp"

core::Game::Game() {
    ecsManager_ = std::make_unique<ecs::Manager>();
    assetManager_ = std::make_unique<assets::Manager>();
    physicsSystem_ = std::make_unique<ecs::PhysicsSystem>();
}

core::Game::~Game() {
    std::cout << "[GAME] Shutting down game and cleaning up resources..." << std::endl;

    currentScene_.reset();
    assetManager_.reset(); // destroy first the textures since it need the renderer pointer to be alive
    
    // ecs
    physicsSystem_.reset();
    ecsManager_.reset();
    
    // when destroying renderer system we destroy the SDL_renderer and window, so
    // we can safely do SDL Quit after that
    rendererSystem_.reset();
    
    SDL_Quit();
}

bool core::Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    rendererSystem_ = std::make_unique<renderer::Renderer>(title, width, height);

    changeScene(std::make_unique<scene::LuaScene>(*ecsManager_, *assetManager_, *physicsSystem_, *rendererSystem_, *this, "assets/scripts/main.lua"));

    isRunning_ = true;
    return true;
}

void core::Game::run() {
    const int FPS = 144;
    const float frameDelay = 1000.0f / FPS;

    Uint64 lastTime = SDL_GetTicks();
    float deltaTime = 0.0f;

    while (isRunning_) {
        Uint64 frameStart = SDL_GetTicks();

        Uint64 currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        if (deltaTime > 0.1f) {
            deltaTime = 0.1f; // cap deltaTime to avoid spiral of death
        }

        handleEvents();
        handleInput();
        update(deltaTime);
        render(deltaTime);

        Uint32 frameTime = (Uint32)(SDL_GetTicks() - frameStart);
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void core::Game::stop() {
    isRunning_ = false;
}

void core::Game::changeScene(std::unique_ptr<scene::Scene> newScene) {
    if (currentScene_) {
        currentScene_->onExit();
    }
    currentScene_ = std::move(newScene);
    if (currentScene_) {
        currentScene_->onEnter();
    }
}

void core::Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            stop();
        }
        if (currentScene_) {
            currentScene_->handleEvents(event);
        }
    }
}

void core::Game::handleInput() {
    // we can handle global input here, for example if we want to quit the game when the user presses escape
    const bool* state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_ESCAPE]) {
        std::cout << "[GAME::handleInput] Calling stop()" << std::endl;
        stop();
    }

    if (currentScene_) {
        currentScene_->handleInput();
    }
}

void core::Game::update(float deltaTime) {
    if (currentScene_) {
        currentScene_->onUpdate(deltaTime);
    }
    float time_before_physics = SDL_GetTicks();
    physicsSystem_->update(*ecsManager_, deltaTime);
    float time_after_physics = SDL_GetTicks();
    // std::cout << "Physics update took " << (time_after_physics - time_before_physics) << " ms" << std::endl;
}

void core::Game::render(float deltaTime) {
    rendererSystem_->update(*ecsManager_, deltaTime);
}