#define _USE_MATH_DEFINES
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <math.h>


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
// глобальная переменная для хранения индексов квадрата сетки
int x_index, y_index;
// обработчик события нажатия мыши
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Window dimensions
GLuint WIDTH = 600, HEIGHT = 800;
const int x_parts = 5, y_parts = 7;

class Renderer {


private:
    GLuint VBO, VAO;
    float dx, dy;

    void init(GLfloat* vertices, GLuint size) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(VAO);
    }

    void draw_line(int size) {
        glDrawArrays(GL_LINE_LOOP, 0, size / (3 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }

    void ReuleauxTriangle(GLfloat x, GLfloat y) {
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

        init(vertices, sizeof(vertices));
        glLineWidth(3);
        glDrawArrays(GL_TRIANGLE_FAN, 0, pointCount * 2);
        glBindVertexArray(0);
    }

    void drawSquareLines(GLfloat x, GLfloat y) {

        // Рисуем квадрат
        GLfloat vertices[] = {
            // Positions
            x - dx / 2, y - dy / 2, 0.0f,
            x + dx / 2, y - dy / 2, 0.0f,
            x + dx / 2, y + dy / 2, 0.0f,
            x - dx / 2, y + dy / 2, 0.0f,
        };
        init(vertices, sizeof(vertices));
        glLineWidth(10);
        draw_line(sizeof(vertices));
    }

    void FillEllipse(GLfloat x, GLfloat y) {
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

        init(vertices, sizeof(vertices));
        glLineWidth(3);
        glDrawArrays(GL_TRIANGLE_FAN, 0, pointCount * 2);
        glBindVertexArray(0);
    }

    void drawTriangle(GLfloat x, GLfloat y) {

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

    void drawRhombus(GLfloat x, GLfloat y) {
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

    void drawSquare(GLfloat x, GLfloat y) {

        // Рисуем квадрат
        GLfloat vertices[] = {
            // Positions
            x - dx / 2, y - dy / 2, 0.0f,
            x + dx / 2, y - dy / 2, 0.0f,
            x + dx / 2, y + dy / 2, 0.0f,
            x - dx / 2, y + dy / 2, 0.0f,
        };
        init(vertices, sizeof(vertices));
        glLineWidth(3);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);
    }

    GLfloat animate_step;

public:

    Renderer(GLuint VBO_, GLuint VAO_) : VBO(VBO_), VAO(VAO_) {
        glBindVertexArray(VAO);
        dx = 0, 5; dy = 0, 5;
        animate_step = 0;
    }

    void drawBorderLines() {
        GLfloat vertices[] = {
            // Positions
            -1.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
        };
        init(vertices, sizeof(vertices));
        glLineWidth(10);
        draw_line(sizeof(vertices));
    }

    void drawGrid(int x, int y) {

        glLineWidth(3);
        dx = 2.0f / (x + 2);
        dy = 2.0f / (y + 3);

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

    void drawRhombus(int x, int y) {
        drawRhombus((GLfloat)x * dx - 1 + dx / 2, (GLfloat)y * dy - 1 + dy / 2);
    }

    void drawSquare(int x, int y) {
        drawSquare((GLfloat)x * dx - 1 + dx / 2, (GLfloat)y * dy - 1 + dy / 2);
    }

    void drawTriangle(int x, int y) {
        drawTriangle((GLfloat)x * dx - 1 + dx / 2, (GLfloat)y * dy - 1 + dy / 2);
    }

    void drawSquareLines(int x, int y) {
        drawSquareLines((GLfloat)x * dx - 1 + dx / 2, (GLfloat)y * dy - 1 + dy / 2);
    }

    void FillEllipse(int x, int y) {
        FillEllipse((GLfloat)x * dx - 1 + dx / 2, (GLfloat)y * dy - 1 + dy / 2);
    }

    void ReuleauxTriangle(int x, int y) {
        ReuleauxTriangle((GLfloat)x * dx - 1 + dx / 2, (GLfloat)y * dy - 1 + dy / 2);
    }

    void AnimateReuleauxTriangle(int x, int y, int x_new, int y_new, bool flag) {
        GLfloat delta_x = (GLfloat)(x - x_new) * dx;
        GLfloat delta_y = (GLfloat)(y - y_new) * dy;
        GLfloat steps = 20;
        if (animate_step < steps) {
            animate_step += 1;
            if (x < x_new)
                if (y < y_new)
                    ReuleauxTriangle((GLfloat)x * dx - 1 + dx / 2 + animate_step * delta_x / steps, (GLfloat)y * dy - 1 + dy / 2 + animate_step * delta_y / steps);
                else
                    ReuleauxTriangle((GLfloat)x * dx - 1 + dx / 2 + animate_step * delta_x / steps, (GLfloat)y * dy - 1 + dy / 2 - animate_step * delta_y / steps);
            else
                if (y < y_new)
                    ReuleauxTriangle((GLfloat)x * dx - 1 + dx / 2 - animate_step * delta_x / steps, (GLfloat)y * dy - 1 + dy / 2 + animate_step * delta_y / steps);
                else
                    ReuleauxTriangle((GLfloat)x * dx - 1 + dx / 2 - animate_step * delta_x / steps, (GLfloat)y * dy - 1 + dy / 2 - animate_step * delta_y / steps);
        }
        else if (animate_step == steps)
            animate_step = 0;

        if (!flag) {
            AnimateReuleauxTriangle(x_new, y_new, x, y, true);
        }
    }

};


// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);


    // Build and compile our shader program
    Shader ourShader("shaders/default.vs", "shaders/default.frag");
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    Renderer ren = Renderer(VBO, VAO);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the triangle
        ourShader.Use();
        ren.drawGrid(x_parts, y_parts);
        ren.drawBorderLines();
        ren.drawSquare(1, 3);
        ren.drawRhombus(3, 4);
        ren.drawTriangle(5, 2);
        ren.FillEllipse(4, 3);
        ren.ReuleauxTriangle(3, 6);
        ren.drawSquareLines(x_index, y_index);
        ren.AnimateReuleauxTriangle(2, 5, 3, 5, true);
        //ren.drawGrid(x_parts, y_parts);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// обработчик события нажатия мыши
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // получаем текущие координаты курсора мыши
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // определяем, в каком квадрате сетки находится курсор мыши
        x_index = (int)(xpos / (WIDTH / (x_parts + 2)));
        y_index = (int)(ypos / (HEIGHT / (y_parts + 3)));
        y_index = y_parts + 2 - y_index;
        // выводим надпись с индексами квадрата сетки
        std::cout << "Mouse is in square: " << x_index << ", " << y_index << std::endl;
    }
}

