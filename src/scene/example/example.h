/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#include <scene/scene.h>

class Example : public Scene {
   public:
    Example(Game*);
    void draw() override;
};