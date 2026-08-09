#include "Game.h"

int main(int /*argc*/, char* /*argv*/[]) {
    Game game;

    if (!game.init("Tree Swinging", 1280, 720)) {
        return 1;
    }

    game.run();
    game.shutdown();

    return 0;
}
