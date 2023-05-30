/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
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