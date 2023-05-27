/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#ifndef _HOMD_GRAPHICS
#define _HOMD_GRAPHICS

class Game;

class Graphics {
   public:
    Game* game;
    Graphics() = default;
    virtual ~Graphics() = default;
    virtual void drawSomething() = 0;
};

#endif