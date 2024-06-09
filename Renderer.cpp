#define _USE_MATH_DEFINES
#include "Renderer.h"
#include <math.h>
#include <vector>

void Renderer::init3(GLfloat* vertices, GLfloat size) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO);
}

void Renderer::draw_line(int size) {
    glDrawArrays(GL_LINE_LOOP, 0, size / (3 * sizeof(GLfloat)));
    glBindVertexArray(0);
}

void Renderer::_drawReule(GLfloat x, GLfloat y) {
    const int pointCount = 360;
    const float step = float(2 * M_PI) / pointCount;

    GLfloat vertices[pointCount * 2 * 3]; // 2 вершины на отрезок, 3 координаты на вершину
    for (int i = 0; i < pointCount; i++) {
        float t = i * step;
        float p = 9 + cosf(3 * t);
        float dp = -3 * sinf(3 * t);

        float x1 = p * cosf(t) - dp * sinf(t) + x;
        float y1 = p * sinf(t) + dp * cosf(t) + y;
        float x2 = p * cosf(t + step) - dp * sinf(t + step) + x;
        float y2 = p * sinf(t + step) + dp * cosf(t + step) + y;

        vertices[i * 6] = x1 * dx / 20 + x;
        vertices[i * 6 + 1] = y1 * dy / 20 + y;
        vertices[i * 6 + 2] = 0.0f;

        vertices[i * 6 + 3] = x2 * dx / 20 + x;
        vertices[i * 6 + 4] = y2 * dy / 20 + y;
        vertices[i * 6 + 5] = 0.0f;
    }

    init3(vertices, sizeof(vertices));
    glLineWidth(3);
    glDrawArrays(GL_TRIANGLE_FAN, 0, pointCount * 2);
    glBindVertexArray(0);
}

void Renderer::_drawSquareLines(GLfloat x, GLfloat y) {

    // Рисуем квадрат
    GLfloat vertices[] = {
        // Positions
        x - dx / 2, y - dy / 2, 0.0f,
        x + dx / 2, y - dy / 2, 0.0f,
        x + dx / 2, y + dy / 2, 0.0f,
        x - dx / 2, y + dy / 2, 0.0f,
    };
    init3(vertices, sizeof(vertices));
    glLineWidth(10);
    draw_line(sizeof(vertices));
}

void Renderer::_drawEllipse(GLfloat x, GLfloat y) {
    const int pointCount = 10;
    const float step = float(2 * M_PI) / pointCount;

    GLfloat vertices[pointCount * 2 * 3]; // 2 вершины на отрезок, 3 координаты на вершину
    for (int i = 0; i < pointCount; i++) {
        float angle = i * step;
        float x1 = dx / 2 * cosf(angle) + x;
        float y1 = dy / 2 * sinf(angle) + y;
        float x2 = dx / 2 * cosf(angle + step) + x;
        float y2 = dy / 2 * sinf(angle + step) + y;

        vertices[i * 6] = x1;
        vertices[i * 6 + 1] = y1;
        vertices[i * 6 + 2] = 0.0f;

        vertices[i * 6 + 3] = x2;
        vertices[i * 6 + 4] = y2;
        vertices[i * 6 + 5] = 0.0f;
    }

    init3(vertices, sizeof(vertices));
    glLineWidth(3);
    glDrawArrays(GL_TRIANGLE_FAN, 0, pointCount * 2);
    glBindVertexArray(0);
}

void Renderer::_drawTriangle(GLfloat x, GLfloat y) {

    GLfloat vertices[] = {
        // Positions
        x + dx / 2, y - dy / 2, 0.0f,
        x - dx / 2, y - dy / 2, 0.0f,
        x, y + dy / 2, 0.0f
    };

    init3(vertices, sizeof(vertices));
    glLineWidth(3);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void Renderer::_drawRhombus(GLfloat x, GLfloat y) {
    GLfloat vertices[] = {
        // Positions
        x + dx / 2, y, 0.0f,
        x, y + dy / 2, 0.0f,
        x - dx / 2, y, 0.0f,
        x, y - dy / 2, 0.0f,
    };
    init3(vertices, sizeof(vertices));
    glLineWidth(3);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void Renderer::_drawSquare(GLfloat x, GLfloat y) {

    // Рисуем квадрат
    GLfloat vertices[] = {
        // Positions
        x - dx / 2, y - dy / 2, 0.0f,
        x + dx / 2, y - dy / 2, 0.0f,
        x + dx / 2, y + dy / 2, 0.0f,
        x - dx / 2, y + dy / 2, 0.0f,
    };
    init3(vertices, sizeof(vertices));
    glLineWidth(3);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void Renderer::_AnimateDrawObject(int x, int y, int x_new, int y_new, bool flag, std::function<void(GLfloat, GLfloat)> draw_object) {
    GLfloat delta_x = (GLfloat)(x_new - x) * dx;
    GLfloat delta_y = (GLfloat)(y_new - y) * dy;
    if ((animate_step < steps) && (flag)) {
        draw_object((GLfloat)x * dx - 1 + dx / 2 + animate_step * delta_x / steps, (GLfloat)y * dy - 1 + dy / 2 + animate_step * delta_y / steps);
        animate_step += 1;
    }
    else if (animate_step == steps) {
        animate_step = 0;
    }
    else if (animate_step < steps / 2) {
        draw_object((GLfloat)x * dx - 1 + dx / 2 + animate_step * delta_x / steps * 2, (GLfloat)y * dy - 1 + dy / 2 + animate_step * delta_y / steps * 2);
        animate_step += 1;
    }
    else if (animate_step > steps / 2) {
        draw_object((GLfloat)x_new * dx - 1 + dx / 2 - (animate_step - steps / 2) * delta_x / steps * 2, (GLfloat)y_new * dy - 1 + dy / 2 - (animate_step - steps / 2) * delta_y / steps * 2);
        animate_step += 1;
    }
}

Renderer::Renderer(GLuint VBO_, GLuint VAO_, int x, int y) : VBO(VBO_), VAO(VAO_) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    animate_step = 0;

    Renderer::dx = 2.0f / ((GLfloat)x + 2.0f);
    Renderer::dy = 2.0f / ((GLfloat)y + 3.0f);
}

Renderer::Renderer(GLuint VBO_, GLuint VAO_, int x, int y, GLint vertexColorLocation_) : VBO(VBO_), VAO(VAO_), vertexColorLocation(vertexColorLocation_) {
    glBindVertexArray(VAO);
    animate_step = 0;

    Renderer::dx = 2.0f / ((GLfloat)x + 2.0f);
    Renderer::dy = 2.0f / ((GLfloat)y + 3.0f);
}

void Renderer::drawBorderLines() {
    GLfloat vertices[] = {
        // Positions
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };
    init3(vertices, sizeof(vertices));
    glLineWidth(10);
    draw_line(sizeof(vertices));
}

void Renderer::drawGrid(int x, int y) {

    glLineWidth(3);

    // Рисуем вертикальные линии
    float y1 = -1.0f + dy;
    float y2 = 1.0f - 2 * dy;
    for (int i = 1; i <= x + 1; i++) {
        float x1 = -1.0f + dx * i;
        float x2 = x1;

        GLfloat vertices[] = {
            x1, y1, 0.0f,
            x2, y2, 0.0f
        };
        init3(vertices, sizeof(vertices));
        glDrawArrays(GL_LINES, 0, 2);
    }

    // Рисуем горизонтальные линии
    float x1 = -1.0f + dx;
    float x2 = 1.0f - dx;
    for (int i = 1; i <= y + 1; i++) {
        float y1 = -1.0f + dy * i;
        float y2 = y1;

        GLfloat vertices[] = {
            x1, y1, 0.0f,
            x2, y2, 0.0f
        };
        init3(vertices, sizeof(vertices));
        glDrawArrays(GL_LINES, 0, 2);
    }
}

void Renderer::drawSquareLines(int x, int y) {
    _drawSquareLines((GLfloat)x * dx - 1 + dx / 2, (GLfloat)y * dy - 1 + dy / 2);
}

ShapeMap Renderer::createShapeMap() {
    ShapeMap shapeMap;
    shapeMap[REULE] = std::bind(&Renderer::_drawReule, this, std::placeholders::_1, std::placeholders::_2);
    shapeMap[SQUARE] = std::bind(&Renderer::_drawSquare, this, std::placeholders::_1, std::placeholders::_2);
    shapeMap[ELLIPSE] = std::bind(&Renderer::_drawEllipse, this, std::placeholders::_1, std::placeholders::_2);
    shapeMap[TRIANGLE] = std::bind(&Renderer::_drawTriangle, this, std::placeholders::_1, std::placeholders::_2);
    shapeMap[RHOMBUS] = std::bind(&Renderer::_drawRhombus, this, std::placeholders::_1, std::placeholders::_2);
    return shapeMap;
}

ColorMap Renderer::createColorMap() {
    ColorMap colorMap;

    colorMap[Red] = [this]() { glUniform4f(vertexColorLocation, 1.0f, 0.0f, 0.0f, 1.0f); };
    colorMap[Green] = [this]() { glUniform4f(vertexColorLocation, 0.0f, 1.0f, 0.0f, 1.0f); };
    colorMap[Blue] = [this]() { glUniform4f(vertexColorLocation, 0.0f, 0.0f, 1.0f, 1.0f); };
    colorMap[Yellow] = [this]() { glUniform4f(vertexColorLocation, 1.0f, 1.0f, 0.0f, 1.0f); };
    colorMap[Black] = [this]() { glUniform4f(vertexColorLocation, 0.0f, 0.0f, 0.0f, 1.0f); };
    colorMap[Cyan] = [this]() { glUniform4f(vertexColorLocation, 0.0f, 1.0f, 1.0f, 1.0f); }; // Add this line
    colorMap[Magenta] = [this]() { glUniform4f(vertexColorLocation, 1.0f, 0.0f, 1.0f, 1.0f); }; // Add this line
    colorMap[Brown] = [this]() { glUniform4f(vertexColorLocation, 1.0f, 0.5f, 0.0f, 1.0f); }; // Add this line
    colorMap[White] = [this]() { glUniform4f(vertexColorLocation, 1.0f, 1.0f, 1.0f, 1.0f); }; // Add this line
    colorMap[Gray] = [this]() { glUniform4f(vertexColorLocation, 0.5f, 0.5f, 0.5f, 1.0f); }; // Add this line
    colorMap[Pink] = [this]() { glUniform4f(vertexColorLocation, 1.0f, 0.7f, 0.7f, 1.0f); }; // Add this line
    colorMap[Orange] = [this]() { glUniform4f(vertexColorLocation, 1.0f, 0.5f, 0.0f, 1.0f); }; // Add this line

    return colorMap;
}

void Renderer::drawObject(int x, int y, ShapeType shapeType) {
    ShapeMap shapeMap = createShapeMap();
    auto draw_object = shapeMap[shapeType];
    draw_object((GLfloat)x * dx - 1 + dx / 2, (GLfloat)y * dy - 1 + dy / 2);
}

void Renderer::drawObject(int x, int y, ShapeType shapeType, GLint vertexColorLocation) {
    ShapeMap shapeMap = createShapeMap();
    auto draw_object = shapeMap[shapeType];
    glUniform4f(vertexColorLocation, 1.0f, 1.0f, 0.0f, 1.0f);
    draw_object((GLfloat)x * dx - 1 + dx / 2, (GLfloat)y * dy - 1 + dy / 2);
}

void Renderer::drawObject(int x, int y, ShapeType shapeType, Color color) {
    ShapeMap shapeMap = createShapeMap();
    auto draw_object = shapeMap[shapeType];
    ColorMap colorMap = createColorMap();
    auto color_function = colorMap[color];
    color_function();
    draw_object((GLfloat)x * dx - 1 + dx / 2, (GLfloat)y * dy - 1 + dy / 2);
}

void Renderer::drawObject(GameObject obj) {
    drawObject(obj.x, obj.y, obj.shapeType, obj.color);
}

void Renderer::Animate(int x, int y, int x_new, int y_new, bool flag, ShapeType shapeType) {
    ShapeMap shapeMap = createShapeMap();
    auto draw_object = shapeMap[shapeType];
    _AnimateDrawObject(x, y, x_new, y_new, flag, draw_object);
}

void Renderer::Animate(int x, int y, int x_new, int y_new, bool flag, ShapeType shapeType1, ShapeType shapeType2) {
    ShapeMap shapeMap = createShapeMap();
    auto draw_object1 = shapeMap[shapeType1];
    auto draw_object2 = shapeMap[shapeType2];
    _AnimateDrawObject(x, y, x_new, y_new, flag, draw_object1);
    _AnimateDrawObject(x_new, y_new, x, y, flag, draw_object2);
}

void Renderer::Animate(int x, int y, int x_new, int y_new, bool flag, ShapeType shapeType, Color color) {
    ShapeMap shapeMap = createShapeMap();
    auto draw_object = shapeMap[shapeType];
    ColorMap colorMap = createColorMap();
    auto color_function = colorMap[color];
    color_function();
    _AnimateDrawObject(x, y, x_new, y_new, flag, draw_object);
}

void Renderer::Animate(int x, int y, int x_new, int y_new, bool flag, ShapeType shapeType1, Color color1, ShapeType shapeType2, Color color2) {

    ShapeMap shapeMap1 = createShapeMap();
    ColorMap colorMap1 = createColorMap();
    auto draw_object1 = shapeMap1[shapeType1];
    auto color_function1 = colorMap1[color1];
    color_function1();
    _AnimateDrawObject(x, y, x_new, y_new, flag, draw_object1);

    ShapeMap shapeMap2 = createShapeMap();
    ColorMap colorMap2 = createColorMap();
    auto color_function2 = colorMap2[color2];
    color_function2();
    auto draw_object2 = shapeMap2[shapeType2];
    _AnimateDrawObject(x_new, y_new, x, y, flag, draw_object2);
}

void Renderer::Animate(GameObject obj, int x_new, int y_new, bool flag) {
    Animate(obj.x, obj.y, x_new, y_new, flag, obj.shapeType, obj.color);
}

void Renderer::Animate(GameObject obj1, GameObject obj2, bool flag) {
    Animate(obj1.x, obj1.y, obj2.x, obj2.y, flag, obj1.shapeType, obj1.color, obj2.shapeType, obj2.color);
}

