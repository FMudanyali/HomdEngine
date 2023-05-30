/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
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