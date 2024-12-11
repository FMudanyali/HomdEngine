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

#ifndef _HOMD_WINDOW
#define _HOMD_WINDOW

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

#define SDL_FLAGS SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER
#define SDL_WINDOW_FLAGS                                                  \
    SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_MAXIMIZED | \
        SDL_WINDOW_RESIZABLE

#define WINDOW_TITLE "Homd Engine"
#define WINDOW_INITIAL_W 960
#define WINDOW_INITIAL_H 540

class Game;

class Window {
    Game* game;
    int width;
    int height;

   public:
    SDL_Window* window;

    Window(Game*);
    ~Window();
    void updateDimensions();
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;
};

#endif