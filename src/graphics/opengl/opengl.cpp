/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#include <GLES3/gl3.h>
#define GLEW_STATIC
#include <SDL2/SDL_video.h>
#include <GL/glew.h>

#include <graphics/opengl/opengl.h>
#include <game/game.h>
#include <window/window.h>
#include <string>
#include <iostream>

static const GLchar* vertexShaderSrc = R"(
    attribute vec4 position;
    void main() {
        gl_Position = vec4(position.xyz, 1.0);
    }
)";

static const GLchar* fragmentShaderSrc = R"(
    precision mediump float;
    void main() {
        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
)";

static GLfloat vertices[] = {0.0, 0.8, -0.8, -0.8, 0.8, -0.8};

OpenGL::OpenGL(Game* pGame) {
    glewExperimental = GL_TRUE;
    this->game = pGame;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetSwapInterval(0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    this->context = SDL_GL_CreateContext(this->game->pWindow->window);

    GLenum err = glewInit();
    std::cout << glewGetErrorString(err);

    GLuint vertexArrObj;
    glGenVertexArrays(1, &vertexArrObj);
    glBindVertexArray(vertexArrObj);

    GLuint vertexBufObj;
    glGenBuffers(1, &vertexBufObj);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    GLint posAttribute = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttribute);
    glVertexAttribPointer(posAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void OpenGL::drawSomething() {
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    SDL_GL_SwapWindow(this->game->pWindow->window);
}