/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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