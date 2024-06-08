#define _USE_MATH_DEFINES
#include "Renderer.h"
#include <math.h>


void Renderer::init(GLfloat* vertices, GLfloat size) {
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

        vertices[i * 6] = x1*dx/20+x;
        vertices[i * 6 + 1] = y1*dy/20+y;
        vertices[i * 6 + 2] = 0.0f;

        vertices[i * 6 + 3] = x2*dx/20+x;
        vertices[i * 6 + 4] = y2*dy/20+y;
        vertices[i * 6 + 5] = 0.0f;
    }

    init(vertices, sizeof(vertices));
    glLineWidth(3);
    glDrawArrays(GL_TRIANGLE_FAN, 0, pointCount * 2);
    glBindVertexArray(0);
}

void Renderer::_drawSquareLines(GLfloat x, GLfloat y) {
    
    // Рисуем квадрат
    GLfloat vertices[] = {
        // Positions
        x-dx/2, y-dy/2, 0.0f,
        x+dx/2, y-dy/2, 0.0f,
        x+dx/2, y+dy/2, 0.0f,
        x-dx/2, y+dy/2, 0.0f,
    };
    init(vertices, sizeof(vertices));        
    glLineWidth(10);
    draw_line(sizeof(vertices));
}

void Renderer::_drawEllipse(GLfloat x, GLfloat y) {
    const int pointCount = 10;
    const float step = float(2 * M_PI) / pointCount;

    GLfloat vertices[pointCount * 2 * 3]; // 2 вершины на отрезок, 3 координаты на вершину
        for (int i = 0; i < pointCount; i++) {
            float angle = i * step;
            float x1 = dx/2 * cosf(angle) + x;
            float y1 = dy/2 * sinf(angle) + y;
            float x2 = dx/2 * cosf(angle + step) + x;
            float y2 = dy/2 * sinf(angle + step) + y;

            vertices[i * 6] = x1;
            vertices[i * 6 + 1] = y1;
            vertices[i * 6 + 2] = 0.0f;

            vertices[i * 6 + 3] = x2;
            vertices[i * 6 + 4] = y2;
            vertices[i * 6 + 5] = 0.0f;
        }

        init(vertices, sizeof(vertices));
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

    init(vertices, sizeof(vertices));
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
    init(vertices, sizeof(vertices));
    glLineWidth(3);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void Renderer::_drawSquare(GLfloat x, GLfloat y) {

    // Рисуем квадрат
    GLfloat vertices[] = {
        // Positions
        x-dx/2, y-dy/2, 0.0f,
        x+dx/2, y-dy/2, 0.0f,
        x+dx/2, y+dy/2, 0.0f,
        x-dx/2, y+dy/2, 0.0f,
    };
    init(vertices, sizeof(vertices));
    glLineWidth(3);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

GLfloat animate_step;

void Renderer::_AnimateDrawObject(int x, int y, int x_new, int y_new, bool flag, std::function<void(GLfloat, GLfloat)> draw_object) {
    GLfloat delta_x = (GLfloat)(x_new-x)*dx;
    GLfloat delta_y = (GLfloat)(y_new-y)*dy;
    GLfloat steps = 100;
    if (animate_step < steps) {            
        draw_object((GLfloat)x*dx-1+dx/2 + animate_step*delta_x/steps , (GLfloat)y*dy-1+dy/2 + animate_step*delta_y/steps);
        animate_step +=1;
    }
    else if (animate_step == steps){
        animate_step = 0;
    } 
    if (!flag) {
            _AnimateDrawObject(x_new, y_new, x, y, true, draw_object);
    }   
}

void Renderer::_AnimateDrawObject(int x, int y, int x_new, int y_new, bool flag, std::function<void(GLfloat, GLfloat)> draw_object, std::function<void(GLfloat, GLfloat)> draw_object2) {
    GLfloat delta_x = (GLfloat)(x_new-x)*dx;
    GLfloat delta_y = (GLfloat)(y_new-y)*dy;
    GLfloat steps = 100;
    if (animate_step < steps) {            
        draw_object((GLfloat)x*dx-1+dx/2 + animate_step*delta_x/steps , (GLfloat)y*dy-1+dy/2 + animate_step*delta_y/steps);
        animate_step +=1;
    }
    else if (animate_step == steps){
        animate_step = 0;
    } 
    if (!flag) {
            _AnimateDrawObject(x_new, y_new, x, y, true, draw_object2);
    }   
}

Renderer::Renderer(GLuint VBO_, GLuint VAO_, int x, int y) : VBO(VBO_), VAO(VAO_) {
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
    init(vertices, sizeof(vertices));
    glLineWidth(10);
    draw_line (sizeof(vertices));
}  

void Renderer::drawGrid(int x, int y) {

    glLineWidth(3);

    // Рисуем вертикальные линии
    float y1 = -1.0f + dy;
    float y2 = 1.0f - 2*dy;
    for (int i = 1; i <= x + 1; i++) {
        float x1 = -1.0f + dx * i;
        float x2 = x1;

        GLfloat vertices[] = {
            x1, y1, 0.0f,
            x2, y2, 0.0f
        };
        init(vertices, sizeof(vertices));
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
        init(vertices, sizeof(vertices));
        glDrawArrays(GL_LINES, 0, 2);
    }
}

void Renderer::drawSquareLines(int x, int y){
    _drawSquareLines ((GLfloat)x*dx-1+dx/2 , (GLfloat)y*dy-1+dy/2);
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

void Renderer::Animate(int x, int y, int x_new, int y_new, bool flag, ShapeType shapeType) {
    ShapeMap shapeMap = createShapeMap();
    auto draw_object = shapeMap[shapeType];
    _AnimateDrawObject(x, y, x_new, y_new, flag, draw_object);
}

void Renderer::drawObject(int x, int y, ShapeType shapeType) {
    ShapeMap shapeMap = createShapeMap();
    auto draw_object = shapeMap[shapeType];
    draw_object((GLfloat)x*dx-1+dx/2 , (GLfloat)y*dy-1+dy/2);
}
