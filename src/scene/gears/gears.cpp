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

    Graphics::enable(GL_CULL_FACE);
    Graphics::enable(GL_DEPTH_TEST);

    pGame->pRenderer->compileShader(vertexShader, GL_VERTEX_SHADER);
    pGame->pRenderer->compileShader(fragmentShader, GL_FRAGMENT_SHADER);
    pGame->pRenderer->bindAttribLoc(0, "position");
    pGame->pRenderer->bindAttribLoc(1, "normal");

    pGame->pRenderer->useProgram();

    modelViewProjectionMatrixLoc =
        pGame->pRenderer->getUniformLoc("ModelViewProjectionMatrix");
    normalMatrixLoc = pGame->pRenderer->getUniformLoc("NormalMatrix");
    lightSrcPosLoc = pGame->pRenderer->getUniformLoc("LightSourcePosition");
    materialColorLoc = pGame->pRenderer->getUniformLoc("MaterialColor");

    Graphics::setUniformValue((GLint)lightSrcPosLoc, lightSourcePos);

    gears[0] = createGear(1.0, 4.0, 1.0, 20, 0.7);
    gears[1] = createGear(0.5, 2.0, 2.0, 10, 0.7);
    gears[2] = createGear(1.3, 2.0, 0.5, 10, 0.7);
}

void GearsScene::fillGearVertex(GLfloat x,
                                GLfloat y,
                                GLfloat z,
                                const GLfloat n[3]) {
    vertex[0][0] = x;
    vertex[0][1] = y;
    vertex[0][2] = z;
    vertex[0][3] = n[0];
    vertex[0][4] = n[1];
    vertex[0][5] = n[2];
    vertex += 1;
}

Point GearsScene::GearPoint(GLfloat radius, int diameter) {
    return Point{(float)((radius)*cosArr[(diameter)]),
                 (float)((radius)*sinArr[(diameter)])};
}

void GearsScene::setNormal(GLfloat x, GLfloat y, GLfloat z) {
    normal[0] = x;
    normal[1] = y;
    normal[2] = z;
}

void GearsScene::gearVert(int point, int sign, GLfloat gearWidth) {
    fillGearVertex(points[(point)].x, points[(point)].y,
                   (GLfloat)(sign)*gearWidth * 0.5F, normal);
}

void GearsScene::startStrip(Gear* gear, int& currentStrip) {
    gear->strips[currentStrip].first = (GLint)(vertex - gear->vertices);
}

void GearsScene::endStrip(Gear* gear, int& currentStrip) {
    int _tmp = (GLint)(vertex - gear->vertices);
    gear->strips[currentStrip].count = _tmp - gear->strips[currentStrip].first;
    currentStrip++;
}

void GearsScene::quadWithNormal(int p1, int p2, GLfloat gearWidth) {
    setNormal((points[(p1)].y - points[(p2)].y),
              -(points[(p1)].x - points[(p2)].x), 0);
    gearVert((p1), -1, gearWidth);
    gearVert((p1), 1, gearWidth);
    gearVert((p2), -1, gearWidth);
    gearVert((p2), 1, gearWidth);
}

Gear* GearsScene::createGear(GLfloat innerRad,
                             GLfloat outerRad,
                             GLfloat gearWidth,
                             GLfloat teeth,
                             GLfloat toothDepth) {
    GLfloat rad0;
    GLfloat rad1;
    GLfloat rad2;
    GLfloat diameter;
    Gear* gear;
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
        double temp = (double)i * 2.0F * M_PI / teeth;
        sincos(temp, &sinArr[0], &cosArr[0]);
        sincos(temp + diameter, &sinArr[1], &cosArr[1]);
        sincos(temp + diameter * 2, &sinArr[2], &cosArr[2]);
        sincos(temp + diameter * 3, &sinArr[3], &cosArr[3]);
        sincos(temp + diameter * 4, &sinArr[4], &cosArr[4]);

        // Create 7 points (x,y coords) that make up a tooth
        points[0] = GearPoint(rad2, 1);
        points[1] = GearPoint(rad2, 2);
        points[2] = GearPoint(rad1, 0);
        points[3] = GearPoint(rad1, 3);
        points[4] = GearPoint(rad0, 0);
        points[5] = GearPoint(rad1, 4);
        points[6] = GearPoint(rad0, 4);

        // Front face
        startStrip(gear, currentStrip);
        setNormal(0, 0, 1.0);
        gearVert(0, +1, gearWidth);
        gearVert(1, +1, gearWidth);
        gearVert(2, +1, gearWidth);
        gearVert(3, +1, gearWidth);
        gearVert(4, +1, gearWidth);
        gearVert(5, +1, gearWidth);
        gearVert(6, +1, gearWidth);
        endStrip(gear, currentStrip);

        // Inner face
        startStrip(gear, currentStrip);
        quadWithNormal(4, 6, gearWidth);
        endStrip(gear, currentStrip);

        // Back face
        startStrip(gear, currentStrip);
        setNormal(0, 0, -1.0);
        gearVert(6, -1, gearWidth);
        gearVert(5, -1, gearWidth);
        gearVert(4, -1, gearWidth);
        gearVert(3, -1, gearWidth);
        gearVert(2, -1, gearWidth);
        gearVert(1, -1, gearWidth);
        gearVert(0, -1, gearWidth);
        endStrip(gear, currentStrip);

        // Outer face
        startStrip(gear, currentStrip);
        quadWithNormal(0, 2, gearWidth);
        endStrip(gear, currentStrip);

        startStrip(gear, currentStrip);
        quadWithNormal(1, 0, gearWidth);
        endStrip(gear, currentStrip);

        startStrip(gear, currentStrip);
        quadWithNormal(3, 1, gearWidth);
        endStrip(gear, currentStrip);

        startStrip(gear, currentStrip);
        quadWithNormal(5, 3, gearWidth);
        endStrip(gear, currentStrip);
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
    memcpy(modelViewProjection, projectionMatrix, sizeof(modelViewProjection));
    Graphics::mulMat4x4(modelViewProjection, modelView);
    Graphics::setUniformMatrixValue((GLint)modelViewProjectionMatrixLoc,
                                    modelViewProjection);

    /*
     * Create and set the NormalMatrix. It's the inverse transpose of the
     * ModelView matrix.
     */
    memcpy(normalMatrix, modelView, sizeof(normalMatrix));
    Graphics::invMat4x4(normalMatrix);
    Graphics::tposeMat4x4(normalMatrix);
    Graphics::setUniformMatrixValue((GLint)normalMatrixLoc, normalMatrix);

    /* Set the gear color */
    Graphics::setUniformValue((GLint)materialColorLoc, color);

    // Draw the triangle strips that comprise the gear
    Graphics::drawArrays(gear->vertexBufObj, GL_TRIANGLE_STRIP, gear->nVertices,
                         0, 2, 0, sizeof(GLfloat) * 3);
}

void GearsScene::reshape() {
    if (width != pGame->pWindow->getWidth() ||
        height != pGame->pWindow->getHeight()) {
        width = pGame->pWindow->getWidth();
        height = pGame->pWindow->getHeight();

        Graphics::calcPersProjTform(projectionMatrix, 60.0,
                                    (float)width / (float)height, 1.0, 1024.0);
        glViewport(0, 0, (GLint)width, (GLint)height);
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
    currentAngle += 70.0F * (GLfloat)dt; /* 70 degrees per second */
    if (currentAngle > 3600.0) {
        currentAngle -= 3600.0;
    }

    pGame->pRenderer->draw();
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