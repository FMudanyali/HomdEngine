/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#include <game/game.h>
#include <scene/example/example.h>
#include <graphics/opengl/opengl.h>

Game::Game() {
    this->pWindow = new Window(this);
    this->pRenderer = new OpenGL(this);
    this->pInput = new Input(this);
    this->scenes.push(new Example(this));
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

        this->scenes.top()->draw();
    }
}