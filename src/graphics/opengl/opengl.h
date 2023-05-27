/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#ifndef _HOMD_GRAPHICS_GL
#define _HOMD_GRAPHICS_GL

#include <SDL2/SDL_video.h>
#include <graphics/graphics.h>

class Window;

class OpenGL : public Graphics {
    SDL_GLContext context = nullptr;

   public:
    OpenGL(Game*);
    ~OpenGL() override = default;

    void setGLContext();
    void drawSomething() override;
};

#endif