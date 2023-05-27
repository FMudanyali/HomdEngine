/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#include <game/game.h>

#ifdef __WIN32
int wmain(int argc, char** argv) {
#else
int main(int argc, char** argv) {
#endif
    auto* game = new Game;
    game->loop();
    return 0;
}