#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <math.h>
#include <functional>
#include "Renderer.h"


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
// глобальная переменная для хранения индексов квадрата сетки
int x_index, y_index;
// обработчик события нажатия мыши
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Window dimensions
GLuint WIDTH = 600, HEIGHT = 800;
const int x_parts = 5, y_parts = 7;

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
    GLint vertexColorLocation = glGetUniformLocation(ourShader.Program, "ourColor");
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    Renderer ren = Renderer(VBO, VAO, (GLfloat)x_parts, (GLfloat)y_parts, vertexColorLocation);

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


        ren.drawObject(1, 3, SQUARE, vertexColorLocation);

        glUniform4f(vertexColorLocation, 1.0f, 0.0f, 1.0f, 1.0f);
        ren.drawObject(3, 4, RHOMBUS, Red);

        glUniform4f(vertexColorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
        ren.drawObject(5, 2, TRIANGLE);

        glUniform4f(vertexColorLocation, 0.0f, 0.5f, 0.5f, 1.0f);
        ren.drawObject(4, 3, ELLIPSE);

        glUniform4f(vertexColorLocation, 1.0f, 0.0f, 0.5f, 1.0f);
        ren.drawObject(3, 6, REULE);

        glUniform4f(vertexColorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
        ren.drawSquareLines(x_index, y_index);
        ren.Animate(2, 5, 3, 5, true, REULE, Blue);
        ren.Animate(4, 3, 2, 7, false, SQUARE, Red, SQUARE, Green);

        //ren.drawGrid(x_parts, y_parts);
        glUniform4f(vertexColorLocation, 0.0f, 0.0f, 0.0f, 1.0f); //черные объекты 


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

