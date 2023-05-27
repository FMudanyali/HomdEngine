/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#ifndef _HOMD_GAME
#define _HOMD_GAME

#include <window/window.h>
#include <input/input.h>
#include <stack>

class Scene;

class Game {
    bool done = false;
    std::stack<Scene*> scenes;

   public:
    Window* pWindow;
    Input* pInput;

    Game();
    ~Game() = default;

    void loop();
};

#endif