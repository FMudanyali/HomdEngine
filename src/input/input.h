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

#ifndef _HOMD_INPUT
#define _HOMD_INPUT

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

class Game;

class Input {
    Game* game = nullptr;
    SDL_Event event;

   public:
    Input(Game*);
    ~Input() = default;
    const Uint8* keys;
    bool pollEvent();
    void pollKeys();
};

#endif