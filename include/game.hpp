#pragma once

#include <SDL3/SDL.h>

namespace uwunya {

typedef struct SDL_State {
    SDL_Window* window;
    SDL_Renderer* renderer;
} SDL_State;

class Game {
public:
    Game(int width, int height);
    ~Game();

    void run();

private:
    void game_loop();
    void handle_events();
    void handle_input();
    void drawing();
    void cleanup();

    void validate_and_move(float x, float y);

    SDL_State state { 0 };

    int width = 1024;
    int height = 768;

    int floor_height = 200;

    bool running = false;

    Ernest ernest;
    float speed_step = 0.001f;
    float current_speed_x = 0.0f;
    float current_speed_y = 0.0f;
    float max_speed = 0.1f;
    float friction = 0.001f;
    float gravity = 0.00981f;
    float jump_speed = 0.5f;
    bool jumping = false;

    std::vector<Floor*> floors;
};

}