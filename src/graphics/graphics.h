/**
 * Copyright (c) 2023, Furkan Mudanyali <fmudanyali@icloud.com>
 * All rights reserved
 */

#ifndef _HOMD_GRAPHICS_GL
#define _HOMD_GRAPHICS_GL

#define GLEW_STATIC

#include <GLES3/gl3.h>
#include <GL/glew.h>
#include <SDL2/SDL_video.h>

class Game;
class Window;

class Graphics {
    Game* pGame;
    SDL_GLContext context = nullptr;
    GLuint program;

   public:
    Graphics(Game*);
    ~Graphics() = default;

    void setGLContext();
    void compileShader(const char* shaderSrc, int shaderType) const;
    void bindAttribLoc(int, const char*) const;
    void useProgram() const;
    GLint getUniformLoc(const char* name) const;
    static void setUniformValue(GLint, const GLfloat[4]);
    static void setUniformMatrixValue(GLint, const GLfloat[16]);

    void draw();

    static void storeVertexBufObj(GLuint&, GLsizeiptr, int*);

    /**
     * Multiplies two 4x4 matrices
     *
     * The result is stored in matrix m.
     *
     * @param m first matrix
     * @param n second matrix
     */
    static void multiply4x4Matrices(GLfloat*, const GLfloat*);

    /**
     * Rotates a 4x4 matrix
     *
     * @param[in,out] m the matrix to rotate
     * @param angle the angle to rotate
     * @param x the x component of the direction to rotate to
     * @param y the y component of the direction to rotate to
     * @param z the z component of the direction to rotate to
     */
    static void rotate4x4Matrix(GLfloat*, GLfloat, GLfloat, GLfloat, GLfloat);

    /**
     * Translates a 4x4 matrix
     *
     * @param[in,out] m the matrix to translate
     * @param x the x component of the direction to translate to
     * @param y the y component of the direction to translate to
     * @param z the z component of the direction to translate to
     */
    static void translate4x4Matrix(GLfloat*, GLfloat, GLfloat, GLfloat);

    /**
     * Creates a 4x4 identity matrix
     *
     * @param m the matrix to convert to an identity matrix
     */
    static void create4x4IdentityMatrix(GLfloat*);

    /**
     * Transposes a 4x4 matrix.
     *
     * @param m the matrix to transpose
     */
    static void transpose4x4Matrix(GLfloat*);

    /**
     * Inverts a 4x4 matrix.
     *
     * @param m the matrix to invert
     */
    static void invert4x4Matrix(GLfloat*);

    /**
     * Calculate a perspective projection transformation.
     *
     * @param m the matrix to save the transformation in
     * @param yFOV the field of view in the y direction
     * @param aspect the view aspect ratio
     * @param zNear the near clipping plane
     * @param zFar the far clipping plane
     */
    static void calcPerspectiveProjectTransformation(GLfloat*,
                                                     GLfloat,
                                                     GLfloat,
                                                     GLfloat,
                                                     GLfloat);
};

#endif