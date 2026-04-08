#include "core/game.hpp"


int main(int argc, char* argv[]) {

    core::Game game;
    game.init("EvE", 800, 600);
    game.run();

    return 0;
}