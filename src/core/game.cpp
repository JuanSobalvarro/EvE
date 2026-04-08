#include "core/game.hpp"

core::Game::Game() {
    ecsManager_ = std::make_unique<ecs::Manager>();
    assetManager_ = std::make_unique<assets::Manager>(renderer_);
    rendererSystem_ = std::make_unique<renderer::Renderer>();
    physicsSystem_ = std::make_unique<ecs::PhysicsSystem>();
}

core::Game::~Game() {
    // clean up SDL resources
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
}

bool core::Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    window_ = SDL_CreateWindow(title, width, height, 0);
    if (!window_) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer_ = SDL_CreateRenderer(window_, nullptr);
    if (!renderer_) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    assetManager_ = std::make_unique<assets::Manager>(renderer_);

    changeScene(std::make_unique<scene::Demo>(*ecsManager_, *assetManager_, *this));

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
    physicsSystem_->update(*ecsManager_, deltaTime);
}

void core::Game::render(float deltaTime) {
    rendererSystem_->update(*ecsManager_, renderer_, deltaTime);
}
