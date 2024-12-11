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

#include <SDL2/SDL_keyboard.h>
#include <game/game.h>
#include <input/input.h>

Input::Input(Game* pGame) {
    this->game = pGame;
}

bool Input::pollEvent() {
    while (SDL_PollEvent(&this->event)) {
        switch (event.type) {
            case SDL_QUIT:
                return true;
        }
    }
    return event.type == SDL_WINDOWEVENT &&
           event.window.event == SDL_WINDOWEVENT_CLOSE &&
           event.window.windowID ==
               SDL_GetWindowID(this->game->pWindow->window);
}

void Input::pollKeys() {
    this->keys = SDL_GetKeyboardState(nullptr);
}