#include "core/game.hpp"

core::Game::Game() {
    ecsManager_ = std::make_unique<ecs::Manager>();
    assetManager_ = std::make_unique<assets::Manager>(renderer_);
    rendererSystem_ = std::make_unique<renderer::Renderer>();
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

    setupDemo();

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

void core::Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            stop();
        }
    }
}

void core::Game::update(float deltaTime) {

    const double now = SDL_GetTicks() / 1000.0; // current time in seconds

    for (auto id : ecsManager_->getActiveEntities()) {
        // Look for our specific background (or anything with a Color and Rectangle)
        if (ecsManager_->hasComponent<ecs::Color>(id) && ecsManager_->hasComponent<ecs::Rectangle>(id)) {
            auto& color = ecsManager_->getComponent<ecs::Color>(id);
            
            color.r = static_cast<Uint8>((std::sin(now) * 0.5f + 0.5f) * 255);
            color.g = static_cast<Uint8>((std::cos(now + M_PI / 2) * 0.5f + 0.5f) * 255);
            color.b = static_cast<Uint8>((std::sin(now + M_PI * 2 / 3) * 0.5f + 0.5f) * 255);
        }
    }
}

void core::Game::render(float deltaTime) {
    rendererSystem_->update(*ecsManager_, renderer_, deltaTime);
}

void core::Game::setupDemo() {
    // background entity
    ecs::EntityId backgroundEntity = ecsManager_->createEntity();
    ecsManager_->addComponent<ecs::Transform>(backgroundEntity, 0.0f, 0.0f);
    ecsManager_->addComponent<ecs::Color>(backgroundEntity, (Uint8)0, (Uint8)0, (Uint8)0); // start with black background

    int w, h;
    SDL_GetWindowSize(window_, &w, &h);
    ecsManager_->addComponent<ecs::Rectangle>(backgroundEntity, (float)w, (float)h, true); // filled rectangle covering the whole screen
}