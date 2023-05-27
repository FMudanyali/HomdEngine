/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#include <game/game.h>
#include <graphics/graphics.h>
#include <scene/example/example.h>

Example::Example(Game* pGame) {
    this->game = pGame;
}

void Example::draw() {
    this->game->pRenderer->drawSomething();
}