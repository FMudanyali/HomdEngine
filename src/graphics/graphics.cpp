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
#include <cstddef>
#include <iostream>
#include <string>

#ifdef DEBUG
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
#endif

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

#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, nullptr);
#endif

    this->program = glCreateProgram();
}

void Graphics::compileShader(const char* shaderSrc, int shaderType) const {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSrc, nullptr);
    glCompileShader(shader);
    glAttachShader(this->program, shader);
#ifdef DEBUG
    char msg[512];
    glGetShaderInfoLog(shader, sizeof msg, nullptr, msg);
    std::cout << "Vertex shader info: " << msg << "\n";
#endif
}

void Graphics::bindAttribLoc(int argIndex, const char* argName) const {
    glBindAttribLocation(this->program, argIndex, argName);
}

void Graphics::useProgram() const {
    glLinkProgram(program);
#ifdef DEBUG
    char msg[512];
    glGetProgramInfoLog(program, sizeof msg, nullptr, msg);
    std::cout << "Program info: " << msg << "\n";
#endif
    glUseProgram(program);
}

GLint Graphics::getUniformLoc(const char* name) const {
    return glGetUniformLocation(this->program, name);
}

void Graphics::setUniformValue(GLint position, const GLfloat value[4]) {
    glUniform4fv(position, 1, value);
}

void Graphics::setUniformMatrixValue(GLint position, const GLfloat value[16]) {
    glUniformMatrix4fv(position, 1, GL_FALSE, value);
}

void Graphics::storeVertexBufObj(GLuint& dest, GLsizeiptr size, int* target) {
    // Store the vertices in a vertex buffer object
    glGenBuffers(1, &dest);
    glBindBuffer(GL_ARRAY_BUFFER, dest);
    glBufferData(GL_ARRAY_BUFFER, size, target, GL_STATIC_DRAW);
}

void Graphics::mulMat4x4(GLfloat* m, const GLfloat* n) {
    GLfloat tmp[16];
    const GLfloat* row;
    const GLfloat* column;
    div_t d;

    for (int i = 0; i < 16; i++) {
        tmp[i] = 0;
        d = div(i, 4);
        row = n + (ptrdiff_t)(d.quot * 4);
        column = m + d.rem;
        for (int j = 0; j < 4; j++) {
            tmp[i] += row[j] * column[(ptrdiff_t)(j * 4)];
        }
    }
    memcpy(m, &tmp, sizeof tmp);
}

void Graphics::rotMat4x4(GLfloat* m,
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

    mulMat4x4(m, r);
}

void Graphics::tlateMat4x4(GLfloat* m, GLfloat x, GLfloat y, GLfloat z) {
    GLfloat t[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, x, y, z, 1};

    mulMat4x4(m, t);
}

void Graphics::identMat4x4(GLfloat* m) {
    GLfloat t[16] = {
        1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    };

    memcpy(m, t, sizeof(t));
}

void Graphics::tposeMat4x4(GLfloat* m) {
    GLfloat t[16] = {m[0], m[4], m[8],  m[12], m[1], m[5], m[9],  m[13],
                     m[2], m[6], m[10], m[14], m[3], m[7], m[11], m[15]};

    memcpy(m, t, sizeof(t));
}

void Graphics::invMat4x4(GLfloat* m) {
    GLfloat t[16];
    identMat4x4(t);

    // Extract and invert the translation part 't'. The inverse of a
    // translation matrix can be calculated by negating the translation
    // coordinates.
    t[12] = -m[12];
    t[13] = -m[13];
    t[14] = -m[14];

    // Invert the rotation part 'r'. The inverse of a rotation matrix is
    // equal to its transpose.
    m[12] = m[13] = m[14] = 0;
    tposeMat4x4(m);

    // inv(m) = inv(r) * inv(t)
    mulMat4x4(m, t);
}

void Graphics::calcPersProjTform(GLfloat* m,
                                 GLfloat yFOV,
                                 GLfloat aspect,
                                 GLfloat zNear,
                                 GLfloat zFar) {
    GLfloat tmp[16];
    identMat4x4(tmp);

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