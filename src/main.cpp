#include "Game.h"
#include <iostream>

int main(int argc, char** argv) {
    (void)argc; (void)argv;

    std::cout << "Starting 3D Basketball Shooting Simulator..." << std::endl;

    Game game(1280, 720);
    if (!game.init()) {
        std::cerr << "Failed to initialize Game engine." << std::endl;
        return -1;
    }

    std::cout << "Running main simulation loop." << std::endl;
    game.run();

    return 0;
}
