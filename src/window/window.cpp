/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#include <window/window.h>

Window::Window(Game* pGame) {
    this->game = pGame;
    if (SDL_Init(SDL_FLAGS) != 0) {
        throw SDL_GetError();
    }

    this->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED, WINDOW_INITIAL_W,
                                    WINDOW_INITIAL_H, SDL_WINDOW_FLAGS);
}

Window::~Window() {
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void Window::updateDimensions() {
    SDL_GL_GetDrawableSize(this->window, &this->width, &this->height);
}

int Window::getWidth() const {
    return this->width;
}

int Window::getHeight() const {
    return this->height;
}