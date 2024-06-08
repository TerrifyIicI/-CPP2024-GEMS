#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <functional>
#include <map>

enum ShapeType {
    REULE,
    SQUARE,
    ELLIPSE,
    TRIANGLE,
    RHOMBUS
};

using DrawFunction = std::function<void(GLfloat, GLfloat)>;
using ShapeMap = std::map<ShapeType, DrawFunction>;

class Renderer {
public:
    Renderer(GLuint VBO, GLuint VAO, int x, int y);
    void drawBorderLines();
    void drawGrid(int x, int y);
    void drawSquareLines(int x, int y);
    void drawObject(int x, int y, ShapeType shapeType);
    void Animate(int x, int y, int x_new, int y_new, bool flag, ShapeType shapeType); 
private:
    GLuint VBO, VAO;
    GLfloat dx, dy;
    void init(GLfloat* vertices, GLfloat size);
    void draw_line(int size);
    void _drawReule(GLfloat x, GLfloat y);
    void _drawSquareLines(GLfloat x, GLfloat y);
    void _drawEllipse(GLfloat x, GLfloat y);
    void _drawTriangle(GLfloat x, GLfloat y);
    void _drawRhombus(GLfloat x, GLfloat y);
    void _drawSquare(GLfloat x, GLfloat y);
    GLfloat animate_step;
    ShapeMap createShapeMap();
    void _AnimateDrawObject(int x, int y, int x_new, int y_new, bool flag, std::function<void(GLfloat, GLfloat)> draw_object);
    void _AnimateDrawObject(int x, int y, int x_new, int y_new, bool flag, std::function<void(GLfloat, GLfloat)> draw_object, std::function<void(GLfloat, GLfloat)> draw_object2);
};

#endif
