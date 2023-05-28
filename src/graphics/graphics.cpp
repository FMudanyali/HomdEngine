/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#include <GLES3/gl3.h>
#include <GL/glew.h>
#include <SDL2/SDL_video.h>

#include <game/game.h>
#include <graphics/graphics.h>
#include <window/window.h>
#include <iostream>
#include <string>

void GLAPIENTRY MessageCallback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar* message,
                                const void* userParam) {
    fprintf(stderr,
            "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
            severity, message);
}

Graphics::Graphics(Game* game) {
    this->pGame = game;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetSwapInterval(0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    this->context = SDL_GL_CreateContext(this->pGame->pWindow->window);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, nullptr);
}

void Graphics::storeVertexBufObj(GLuint& dest, GLsizeiptr size, int* target) {
    // Store the vertices in a vertex buffer object
    glGenBuffers(1, &dest);
    glBindBuffer(GL_ARRAY_BUFFER, dest);
    glBufferData(GL_ARRAY_BUFFER, size, target, GL_STATIC_DRAW);
}

void Graphics::multiply4x4Matrices(GLfloat* m, const GLfloat* n) {
    GLfloat tmp[16];
    const GLfloat* row;
    const GLfloat* column;
    div_t d;

    for (int i = 0; i < 16; i++) {
        tmp[i] = 0;
        d = div(i, 4);
        row = n + d.quot * 4;
        column = m + d.rem;
        for (int j = 0; j < 4; j++) {
            tmp[i] += row[j] * column[j * 4];
        }
    }
    memcpy(m, &tmp, sizeof tmp);
}

void Graphics::rotate4x4Matrix(GLfloat* m,
                               GLfloat angle,
                               GLfloat x,
                               GLfloat y,
                               GLfloat z) {
    double sin;
    double cos;

    sincos(angle, &sin, &cos);
    GLfloat r[16] = {(GLfloat)(x * x * (1 - cos) + cos),
                     (GLfloat)(y * x * (1 - cos) + z * sin),
                     (GLfloat)(x * z * (1 - cos) - y * sin),
                     0,
                     (GLfloat)(x * y * (1 - cos) - z * sin),
                     (GLfloat)(y * y * (1 - cos) + cos),
                     (GLfloat)(y * z * (1 - cos) + x * sin),
                     0,
                     (GLfloat)(x * z * (1 - cos) + y * sin),
                     (GLfloat)(y * z * (1 - cos) - x * sin),
                     (GLfloat)(z * z * (1 - cos) + cos),
                     0,
                     0,
                     0,
                     0,
                     1};

    multiply4x4Matrices(m, r);
}

void Graphics::translate4x4Matrix(GLfloat* m, GLfloat x, GLfloat y, GLfloat z) {
    GLfloat t[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, x, y, z, 1};

    multiply4x4Matrices(m, t);
}

void Graphics::create4x4IdentityMatrix(GLfloat* m) {
    GLfloat t[16] = {
        1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    };

    memcpy(m, t, sizeof(t));
}

void Graphics::transpose4x4Matrix(GLfloat* m) {
    GLfloat t[16] = {m[0], m[4], m[8],  m[12], m[1], m[5], m[9],  m[13],
                     m[2], m[6], m[10], m[14], m[3], m[7], m[11], m[15]};

    memcpy(m, t, sizeof(t));
}

void Graphics::invert4x4Matrix(GLfloat* m) {
    GLfloat t[16];
    create4x4IdentityMatrix(t);

    // Extract and invert the translation part 't'. The inverse of a
    // translation matrix can be calculated by negating the translation
    // coordinates.
    t[12] = -m[12];
    t[13] = -m[13];
    t[14] = -m[14];

    // Invert the rotation part 'r'. The inverse of a rotation matrix is
    // equal to its transpose.
    m[12] = m[13] = m[14] = 0;
    transpose4x4Matrix(m);

    // inv(m) = inv(r) * inv(t)
    multiply4x4Matrices(m, t);
}

void Graphics::calcPerspectiveProjectTransformation(GLfloat* m,
                                                    GLfloat yFOV,
                                                    GLfloat aspect,
                                                    GLfloat zNear,
                                                    GLfloat zFar) {
    GLfloat tmp[16];
    create4x4IdentityMatrix(tmp);

    double sine;
    double cosine;
    double cotangent;
    double deltaZ;
    GLfloat radians = yFOV / 2.0F * (float)M_PI / 180.0F;

    deltaZ = zFar - zNear;
    sincos(radians, &sine, &cosine);

    if ((deltaZ == 0) || (sine == 0) || (aspect == 0)) {
        return;
    }

    cotangent = cosine / sine;

    tmp[0] = (float)cotangent / aspect;
    tmp[5] = (float)cotangent;
    tmp[10] = -(zFar + zNear) / (float)deltaZ;
    tmp[11] = -1;
    tmp[14] = -2 * zNear * zFar / (float)deltaZ;
    tmp[15] = 0;

    memcpy(m, tmp, sizeof(tmp));
}

void Graphics::draw() {
    SDL_GL_SwapWindow(this->pGame->pWindow->window);
}