/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
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