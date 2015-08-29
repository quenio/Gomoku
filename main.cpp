#include "game.h"

int main() {
    Game game {};

    game.startWithPlayer(game.ai());

    return 0;
}
