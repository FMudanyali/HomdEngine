/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#include <game/game.h>
#include <scene/scene.h>

Game::Game() {
    this->pWindow = new Window(this);
    this->pInput = new Input(this);
    this->scenes.push(new Scene);
}

void Game::loop() {
    while (!this->done && !this->scenes.empty()) {
        this->pWindow->updateDimensions();
        this->done = this->pInput->pollEvent();

        if (this->scenes.top()->destroy) {
            delete this->scenes.top();
            this->scenes.pop();
            continue;
        }
    }
}