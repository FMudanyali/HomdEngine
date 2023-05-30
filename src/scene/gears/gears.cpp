/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#include <GLES3/gl3.h>
#include <GL/glew.h>
#include <game/game.h>
#include <graphics/graphics.h>
#include <scene/gears/gears.h>
#include <cstddef>
#include <iostream>
#include <cassert>

static const char* vertexShader = R"(
    attribute vec3 position;
    attribute vec3 normal;

    uniform mat4 ModelViewProjectionMatrix;
    uniform mat4 NormalMatrix;
    uniform vec4 LightSourcePosition;
    uniform vec4 MaterialColor;

    varying vec4 Color;

    void main(void) {
        // Transform the normal to eye coordinates
        vec3 N = normalize(vec3(NormalMatrix * vec4(normal, 1.0)));

        // The LightSourcePosition is the direction for directional light
        vec3 L = normalize(LightSourcePosition.xyz);

        float diffuse = max(dot(N, L), 0.0);
        float ambient = 0.2;

        // Multiply the diffuse value by the vertex color
        // to get the actual color that will be used to draw the vertex with

        // Color = diffuse * MaterialColor;
        Color = vec4((ambient + diffuse) * MaterialColor.xyz, MaterialColor.a);

        gl_Position = ModelViewProjectionMatrix * vec4(position, 1.0);
    }
)";

static const char* fragmentShader = R"(
    #ifdef GL_ES
    precision mediump float;
    #endif
    varying vec4 Color;

    void main(void) {
        gl_FragColor = Color;
    }
)";

GearsScene::GearsScene(Game* pGame) {
    this->pGame = pGame;

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    this->pGame->pRenderer->compileShader(vertexShader, GL_VERTEX_SHADER);
    this->pGame->pRenderer->compileShader(fragmentShader, GL_FRAGMENT_SHADER);
    this->pGame->pRenderer->bindAttribLoc(0, "position");
    this->pGame->pRenderer->bindAttribLoc(1, "normal");

    this->pGame->pRenderer->useProgram();

    this->modelViewProjectionMatrixLoc =
        this->pGame->pRenderer->getUniformLoc("ModelViewProjectionMatrix");
    this->normalMatrixLoc =
        this->pGame->pRenderer->getUniformLoc("NormalMatrix");
    this->lightSrcPosLoc =
        this->pGame->pRenderer->getUniformLoc("LightSourcePosition");
    this->materialColorLoc =
        this->pGame->pRenderer->getUniformLoc("MaterialColor");

    Graphics::setUniformValue((GLint)this->lightSrcPosLoc, lightSourcePos);

    gears[0] = createGear(1.0, 4.0, 1.0, 20, 0.7);
    gears[1] = createGear(0.5, 2.0, 2.0, 10, 0.7);
    gears[2] = createGear(1.3, 2.0, 0.5, 10, 0.7);
}

GearVertex* GearsScene::fillGearVertex(GearVertex* gearVertex,
                                       GLfloat x,
                                       GLfloat y,
                                       GLfloat z,
                                       const GLfloat n[3]) {
    gearVertex[0][0] = x;
    gearVertex[0][1] = y;
    gearVertex[0][2] = z;
    gearVertex[0][3] = n[0];
    gearVertex[0][4] = n[1];
    gearVertex[0][5] = n[2];

    return gearVertex + 1;
}

Gear* GearsScene::createGear(GLfloat innerRad,
                             GLfloat outerRad,
                             GLfloat width,
                             GLfloat teeth,
                             GLfloat toothDepth) {
    GLfloat rad0;
    GLfloat rad1;
    GLfloat rad2;
    GLfloat diameter;

    GearVertex* vertex;
    Gear* gear;

    double sinArr[5];
    double cosArr[5];
    GLfloat normal[3];

    int currentStrip = 0;

    gear = (Gear*)malloc(sizeof *gear);

    // Calculate the radii used in the gear
    rad0 = innerRad;
    rad1 = outerRad - toothDepth / 2.0F;
    rad2 = outerRad + toothDepth / 2.0F;

    diameter = 2.0F * (float)M_PI / teeth / 4.0F;

    // Allocate triangle strip information
    gear->nStrips = STRIPS_PER_TOOTH * teeth;
    gear->strips = (VertexStrip*)calloc(gear->nStrips, sizeof(*gear->vertices));

    // Allocate the vertices
    gear->vertices = (GearVertex*)calloc(VERTICES_PER_TOOTH * teeth,
                                         sizeof(*gear->vertices));
    vertex = gear->vertices;

    // Calculate needed sin/cos for various angles
    for (int i = 0; i < (int)teeth; ++i) {
        sincos((float)i * 2.0F * M_PI / teeth, &sinArr[0], &cosArr[0]);
        sincos((float)i * 2.0F * M_PI / teeth + diameter, &sinArr[1],
               &cosArr[1]);
        sincos((float)i * 2.0F * M_PI / teeth + diameter * 2, &sinArr[2],
               &cosArr[2]);
        sincos((float)i * 2.0F * M_PI / teeth + diameter * 3, &sinArr[3],
               &cosArr[3]);
        sincos((float)i * 2.0F * M_PI / teeth + diameter * 4, &sinArr[4],
               &cosArr[4]);

        // Macros that do shit idk
#define GEAR_POINT(r, da) \
    { (float)((r)*cosArr[(da)]), (float)((r)*sinArr[(da)]) }
#define SET_NORMAL(x, y, z) \
    do {                    \
        normal[0] = (x);    \
        normal[1] = (y);    \
        normal[2] = (z);    \
    } while (0)

#define GEAR_VERT(v, point, sign)                             \
    fillGearVertex((v), points[(point)].x, points[(point)].y, \
                   (sign)*width * 0.5, normal)

#define START_STRIP                                                 \
    do {                                                            \
        gear->strips[currentStrip].first = vertex - gear->vertices; \
    } while (0);

#define END_STRIP                                    \
    do {                                             \
        int _tmp = (vertex - gear->vertices);        \
        gear->strips[currentStrip].count =           \
            _tmp - gear->strips[currentStrip].first; \
        currentStrip++;                              \
    } while (0)

#define QUAD_WITH_NORMAL(p1, p2)                           \
    do {                                                   \
        SET_NORMAL((points[(p1)].y - points[(p2)].y),      \
                   -(points[(p1)].x - points[(p2)].x), 0); \
        vertex = GEAR_VERT(vertex, (p1), -1);              \
        vertex = GEAR_VERT(vertex, (p1), 1);               \
        vertex = GEAR_VERT(vertex, (p2), -1);              \
        vertex = GEAR_VERT(vertex, (p2), 1);               \
    } while (0)

        using Point = struct {
            GLfloat x;
            GLfloat y;
        };

        // Create 7 points (x,y coords) that make up a tooth
        Point points[7] = {
            GEAR_POINT(rad2, 1), GEAR_POINT(rad2, 2), GEAR_POINT(rad1, 0),
            GEAR_POINT(rad1, 3), GEAR_POINT(rad0, 0), GEAR_POINT(rad1, 4),
            GEAR_POINT(rad0, 4),
        };

        // Front face
        START_STRIP;
        SET_NORMAL(0, 0, 1.0);
        vertex = GEAR_VERT(vertex, 0, +1);
        vertex = GEAR_VERT(vertex, 1, +1);
        vertex = GEAR_VERT(vertex, 2, +1);
        vertex = GEAR_VERT(vertex, 3, +1);
        vertex = GEAR_VERT(vertex, 4, +1);
        vertex = GEAR_VERT(vertex, 5, +1);
        vertex = GEAR_VERT(vertex, 6, +1);
        END_STRIP;

        // Inner face
        START_STRIP;
        QUAD_WITH_NORMAL(4, 6);
        END_STRIP;

        // Back face
        START_STRIP;
        SET_NORMAL(0, 0, -1.0);
        vertex = GEAR_VERT(vertex, 6, -1);
        vertex = GEAR_VERT(vertex, 5, -1);
        vertex = GEAR_VERT(vertex, 4, -1);
        vertex = GEAR_VERT(vertex, 3, -1);
        vertex = GEAR_VERT(vertex, 2, -1);
        vertex = GEAR_VERT(vertex, 1, -1);
        vertex = GEAR_VERT(vertex, 0, -1);
        END_STRIP;

        // Outer face
        START_STRIP;
        QUAD_WITH_NORMAL(0, 2);
        END_STRIP;

        START_STRIP;
        QUAD_WITH_NORMAL(1, 0);
        END_STRIP;

        START_STRIP;
        QUAD_WITH_NORMAL(3, 1);
        END_STRIP;

        START_STRIP;
        QUAD_WITH_NORMAL(5, 3);
        END_STRIP;
    }

    gear->nVertices = (int)(vertex - gear->vertices);
    Graphics::storeVertexBufObj(
        gear->vertexBufObj, (GLsizeiptr)(gear->nVertices * sizeof(GearVertex)),
        (int*)gear->vertices);

    return gear;
}

void GearsScene::drawGear(Gear* gear,
                          GLfloat* transform,
                          GLfloat x,
                          GLfloat y,
                          GLfloat angle,
                          const GLfloat color[4]) {
    GLfloat modelView[16];
    GLfloat normalMatrix[16];
    GLfloat modelViewProjection[16];

    // Translate and rotate the gear
    memcpy(modelView, transform, sizeof(modelView));
    Graphics::tlateMat4x4(modelView, x, y, 0);
    Graphics::rotMat4x4(modelView, 2.0F * (float)M_PI * angle / 360.0F, 0, 0,
                        1);

    /* Create and set the ModelViewProjectionMatrix */
    memcpy(modelViewProjection, this->projectionMatrix,
           sizeof(modelViewProjection));
    Graphics::mulMat4x4(modelViewProjection, modelView);
    Graphics::setUniformMatrixValue((GLint)this->modelViewProjectionMatrixLoc,
                                    modelViewProjection);

    /*
     * Create and set the NormalMatrix. It's the inverse transpose of the
     * ModelView matrix.
     */
    memcpy(normalMatrix, modelView, sizeof(normalMatrix));
    Graphics::invMat4x4(normalMatrix);
    Graphics::tposeMat4x4(normalMatrix);
    Graphics::setUniformMatrixValue((GLint)this->normalMatrixLoc, normalMatrix);

    /* Set the gear color */
    Graphics::setUniformValue((GLint)this->materialColorLoc, color);

    /* Set the vertex buffer object to use */
    glBindBuffer(GL_ARRAY_BUFFER, gear->vertexBufObj);

    /* Set up the position of the attributes in the vertex buffer object */
    int bindingIdx = 0;

    glEnableVertexAttribArray(0);
    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(0, bindingIdx);

    glEnableVertexAttribArray(1);
    glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3);
    glVertexAttribBinding(1, bindingIdx);

    glBindVertexBuffer(bindingIdx, gear->vertexBufObj, 0, sizeof(GLfloat) * 6);

    /* Draw the triangle strips that comprise the gear */
    glDrawArrays(GL_TRIANGLE_STRIP, 0, gear->nVertices);

    /* Disable the attributes */
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

void GearsScene::reshape() {
    if (this->width != this->pGame->pWindow->getWidth() ||
        this->height != this->pGame->pWindow->getHeight()) {
        this->width = this->pGame->pWindow->getWidth();
        this->height = this->pGame->pWindow->getHeight();

        Graphics::calcPersProjTform(this->projectionMatrix, 60.0,
                                    (float)this->width / (float)this->height,
                                    1.0, 1024.0);
        glViewport(0, 0, (GLint)this->width, (GLint)this->height);
    }
}

void GearsScene::idle() {
    static int frames = 0;
    static double tRot0 = -1.0;
    static double tRate0 = -1.0;
    double dt;
    double t = SDL_GetTicks() / 1000.0;

    if (tRot0 < 0.0) {
        tRot0 = t;
    }
    dt = t - tRot0;
    tRot0 = t;

    /* advance rotation for next frame */
    this->currentAngle += 70.0F * (GLfloat)dt; /* 70 degrees per second */
    if (this->currentAngle > 3600.0) {
        this->currentAngle -= 3600.0;
    }

    this->pGame->pRenderer->draw();
    frames++;

    if (tRate0 < 0.0) {
        tRate0 = t;
    }
    if (t - tRate0 >= 5.0) {
        auto seconds = (GLfloat)(t - tRate0);
        GLfloat fps = (GLfloat)frames / seconds;
        printf("%d frames in %3.1f seconds = %6.3f FPS\n", frames, seconds,
               fps);
        tRate0 = t;
        frames = 0;
    }
}

void GearsScene::draw() {
    const static GLfloat red[4] = {0.8, 0.1, 0.0, 1.0};
    const static GLfloat green[4] = {0.0, 0.8, 0.2, 1.0};
    const static GLfloat blue[4] = {0.2, 0.2, 1.0, 1.0};
    GLfloat transform[16];
    Graphics::identMat4x4(transform);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Translate and rotate the view */
    Graphics::tlateMat4x4(transform, 0, 0, -20);
    Graphics::rotMat4x4(transform,
                        2.0F * (float)M_PI * viewRotation[0] / 360.0F, 1, 0, 0);
    Graphics::rotMat4x4(transform,
                        2.0F * (float)M_PI * viewRotation[1] / 360.0F, 0, 1, 0);
    Graphics::rotMat4x4(transform,
                        2.0F * (float)M_PI * viewRotation[2] / 360.0F, 0, 0, 1);

    /* Draw the gears */
    drawGear(gears[0], transform, -3.0, -2.0, currentAngle, red);
    drawGear(gears[1], transform, 3.1, -2.0, (float)-2 * currentAngle - 9.0F,
             green);
    drawGear(gears[2], transform, -3.1, 4.2, (float)-2 * currentAngle - 25.0F,
             blue);

    reshape();
    idle();
}