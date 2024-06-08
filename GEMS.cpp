#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 600, HEIGHT = 800;

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

public:

    Renderer(GLuint VBO_, GLuint VAO_) : VBO(VBO_), VAO(VAO_) {
       glBindVertexArray(VAO);
        dx = 0, 5; dy = 0, 5;
    }

    void drawTriangle(GLfloat x, GLfloat y) {
        x = x * dx - 1 + dx / 2;
        y = y * dy - 1 + dy / 2;

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
        x = x * dx - 1 + dx / 2;
        y = y * dy - 1 + dy / 2;
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
        x = x * dx - 1 + dx / 2;
        y = y * dy - 1 + dy / 2;
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
        ren.drawGrid(6, 4);
        ren.drawBorderLines();
        ren.drawSquare(1, 3);
        ren.drawRhombus(3, 4);
        ren.drawTriangle(5, 2);

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

