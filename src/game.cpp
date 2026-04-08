#include "game.hpp"

uwunya::Game::Game(int width, int height) : width(width), height(height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to initialize SDL uwunya", nullptr);
        exit(1);
    }

    // window
    state.window = SDL_CreateWindow("Uwunya", width, height, 0);

    if (!state.window) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to create window uwunya", nullptr);
        cleanup();
        exit(1);
    }

    // renderer
    state.renderer = SDL_CreateRenderer(state.window, nullptr);

    if (!state.renderer) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to create renderer uwunya", nullptr);
        cleanup();
        exit(1);
    }

    ernest.resize(30.0f, 50.0f);
    ernest.move(width / 10, height - floor_height);
}

uwunya::Game::~Game() {
    cleanup();
}

void uwunya::Game::run() {
    game_loop();
}

void uwunya::Game::game_loop() {
    running = true;
    while (running) {
        handle_events();
        handle_input();
        drawing();
    }
}

void uwunya::Game::handle_events() {
    SDL_Event event { 0 };

    while (SDL_PollEvent(&event)) {
        
        switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
        }
    }
}

void uwunya::Game::handle_input() {
    const bool* keys = SDL_GetKeyboardState(nullptr);

    int move_x = 0; // -1 for left, 1 for right, 0 for no movement
    int move_y = 1; // -1 for up (jump), 1 for down (falling), 0 for no movement

    // if (keys[SDL_SCANCODE_W]) {
    //     ernest.move(0.0f, -0.1f);
    // }
    // if (keys[SDL_SCANCODE_S]) {
    //     ernest.move(0.0f, 0.1f);
    // }
    if (keys[SDL_SCANCODE_A]) {
        move_x = -1;
    }
    if (keys[SDL_SCANCODE_D]) {
        move_x = 1;
    }
    if (keys[SDL_SCANCODE_SPACE] && current_speed_y == 0) { // only allow jumping if not already in the air
        move_y = -1;
    }

    // apply friction when on the ground and no horizontal input
    if (move_x == 0 && ernest.get_rect().y + ernest.get_rect().h >= height - floor_height) {
        if (current_speed_x > 0) {
            current_speed_x = std::max(0.0f, current_speed_x - friction);
        } else if (current_speed_x < 0) {
            current_speed_x = std::min(0.0f, current_speed_x + friction);
        }
    } else {
        // apply acceleration
        current_speed_x += move_x * speed_step;
    }

    // apply gravity
    current_speed_y += gravity * jump_speed * move_y;

    validate_and_move(current_speed_x, current_speed_y);
}

void uwunya::Game::validate_and_move(float x, float y) {

    // max speed validations
    current_speed_x = std::min(max_speed, std::abs(current_speed_x)) * (current_speed_x > 0 ? 1 : -1);
    current_speed_y = std::min(max_speed, std::abs(current_speed_y)) * (current_speed_y > 0 ? 1 : -1);

    // floor collision
    if (ernest.get_rect().y + ernest.get_rect().h + y > height - floor_height) {
        y = height - floor_height - ernest.get_rect().y - ernest.get_rect().h;
        current_speed_y = 0;
    }

    // screen walls
    if (ernest.get_rect().x + x < 0) {
        x = -ernest.get_rect().x;
        current_speed_x = 0;
    }
    if (ernest.get_rect().x + ernest.get_rect().w + x > width) {
        x = width - ernest.get_rect().x - ernest.get_rect().w;
        current_speed_x = 0;
    }

    ernest.move(x, y);
}

void uwunya::Game::drawing() {
    const double now = ((double)SDL_GetTicks()) / 1000.0; // from ms to s
    
    const float red = (float) (0.5 + 0.5 * SDL_sin(now));
    const float green = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 1 / 2));
    const float blue = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 3 / 4));
    SDL_SetRenderDrawColorFloat(state.renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT); // sdl alpha op f.. aka 1.0f
    
    SDL_RenderClear(state.renderer); // clear the screen with the current draw color
    
    ernest.draw(state.renderer);

    // remember always show the backbuffer after drawing
    SDL_RenderPresent(state.renderer); // present the backbuffer to the screen, this is where the actual drawing happens
}

void uwunya::Game::cleanup() {
    SDL_DestroyWindow(state.window);
    SDL_DestroyRenderer(state.renderer);
    SDL_Quit();
}
