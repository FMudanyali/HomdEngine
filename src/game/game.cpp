/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#include <game/game.h>
#include <graphics/graphics.h>
#include <scene/gears/gears.h>

Game::Game() {
    this->pWindow = new Window(this);
    this->pRenderer = new Graphics(this);
    this->pInput = new Input(this);
    this->scenes.push(new GearsScene(this));
}

void Game::loop() {
    while (!this->done && !this->scenes.empty()) {
        this->pWindow->updateDimensions();
        this->done = this->pInput->pollEvent();
        this->pInput->pollKeys();

        if (this->scenes.top()->destroy) {
            delete this->scenes.top();
            this->scenes.pop();
            continue;
        }

        this->scenes.top()->draw();
    }
}