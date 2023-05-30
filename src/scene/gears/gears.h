/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#include <GLES3/gl3.h>
#include <GL/glew.h>
#include <scene/scene.h>

#define STRIPS_PER_TOOTH 7
#define VERTICES_PER_TOOTH 34
#define GEAR_VERTEX_STRIDE 6

struct VertexStrip;

// Each vertex consists of GEAR_VERTEX_STRIDE GLfloat attributes
using GearVertex = GLfloat[GEAR_VERTEX_STRIDE];

// Class representing a gear
using Gear = struct {
    // Array of vertices comprising the gear
    GearVertex* vertices;
    // Number of vertices comprising the gear
    int nVertices;
    // Array of triangle strips comprising the gear
    VertexStrip* strips;
    // Number of triangle strips comprising the gear
    int nStrips;
    // Vertex buffer object holding the vertices in the GPU
    GLuint vertexBufObj;
};

using Point = struct {
    GLfloat x;
    GLfloat y;
};

class GearsScene : public Scene {
    // Second set of screen resolution to keep track
    // of window resize
    int width;
    int height;
    // The view rotation [x, y, z]
    GLfloat viewRotation[3] = {20.0, 30.0, 0.0};
    // The gears
    Gear* gears[3];
    // The current gear rotation angle
    GLfloat currentAngle = 0.0;
    // The location of the shader uniforms
    GLuint modelViewProjectionMatrixLoc;
    GLuint normalMatrixLoc;
    GLuint lightSrcPosLoc;
    GLuint materialColorLoc;
    // The projection matrix
    GLfloat projectionMatrix[16];

    GearVertex* vertex;
    double sinArr[5];
    double cosArr[5];
    GLfloat normal[3];
    Point points[7];

    Point GearPoint(GLfloat radius, int diameter);
    void gearVert(int point, int sign, GLfloat gearWidth);
    void setNormal(GLfloat x, GLfloat y, GLfloat z);
    void quadWithNormal(int p1, int p2, GLfloat gearWidth);
    void startStrip(Gear* gear, int& currentStrip);
    void endStrip(Gear* gear, int& currentStrip);

    void idle();
    void reshape();
    void keypress();

    // The direction of the directional light for the scene
    const GLfloat lightSourcePos[4] = {5.0, 5.0, 10.0, 1.0};

    /**
     * Fills a gear vertex.
     *
     * @param gearVertex the vertex to fill
     * @param x the x coordinate
     * @param y the y coordinate
     * @param z the z coordinate
     * @param n pointer to the normal table
     *
     * @return the operation error code
     */
    void fillGearVertex(GLfloat x, GLfloat y, GLfloat z, const GLfloat n[3]);

    /**
     * Create a gear wheel.
     *
     * @param innerRad radius of the hole at the center
     * @param outerRad radius at the center of the teth
     * @param width width of the gear
     * @param teeth the number of teeth
     * @param toothDepth the depth of the teeth
     *
     * @return the pointer to the constructed gear struct
     */
    Gear* createGear(GLfloat innerRad,
                     GLfloat outerRad,
                     GLfloat width,
                     GLfloat teeth,
                     GLfloat toothDepth);

    /**
     * Draws a gear
     *
     * @param transform the current transformation matrix
     * @param x the x pos to draw the gear at
     * @param y the y pos to draw the gear at
     * @param angle the rotation angle of the gear
     * @param color the color of the gear
     */
    void drawGear(Gear*, GLfloat*, GLfloat, GLfloat, GLfloat, const GLfloat[4]);

    // Draws all gears
    void drawAllGears();

   public:
    GearsScene(Game*);
    void draw() override;
};