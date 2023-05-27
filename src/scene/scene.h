/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#ifndef _HOMD_SCENE
#define _HOMD_SCENE

class Scene {
   public:
    bool destroy = false;
    Scene() = default;
    ~Scene() = default;
};

#endif