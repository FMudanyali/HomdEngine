/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#ifndef _HOMD_SCENE
#define _HOMD_SCENE

class Game;

class Scene {
   public:
    Game* pGame;

    bool destroy = false;
    Scene() = default;
    virtual ~Scene() = default;
    virtual void draw() = 0;
};

#endif