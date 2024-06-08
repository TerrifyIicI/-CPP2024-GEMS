#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <math.h>
#include <string>
#include <vector>
#include <list>
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

class GameField {
public:
    GameField(int x_parts, int y_parts) : x_parts(x_parts), y_parts(y_parts) {
        srand(time(0));
        for (int i = 0; i < x_parts; ++i) {
            rows.push_back(std::list<GameObject>());
            for (int j = 1; j <= y_parts; ++j) {
                rows[i].push_back({ i + 1, j, static_cast<Color>(rand() % 3), ShapeType::SQUARE });
            }
        }
    }

    std::vector<std::list<GameObject>> rows;
    int x_parts;
    int y_parts;
};

class GameRenderer : public Renderer, public GameField {
public:
    GameRenderer(GLuint VBO_, GLuint VAO_, int x_parts, int y_parts, GLint vertexColorLocation_)
        : Renderer(VBO_, VAO_, x_parts, y_parts, vertexColorLocation_), GameField(x_parts, y_parts) {
        // Инициализируйте любые дополнительные поля класса GameRenderer здесь
    }

    // Измените этот метод
    void drawField() {

        for (auto& row : rows) { // Используйте ссылку, чтобы избежать копирования
            for (const auto& obj : row) {
                drawObject(obj);
            }
        }

        glUniform4f(vertexColorLocation, 0.0f, 0.0f, 0.0f, 1.0f); //черный
        drawBorderLines();
        drawGrid(x_parts, y_parts);
        drawSquareLines(x_index, y_index);
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
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);


    // Build and compile our shader program
    Shader ourShader("shaders/default.vs", "shaders/default.frag");
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    GLint vertexColorLocation = glGetUniformLocation(ourShader.Program, "ourColor");

    GameRenderer ren = GameRenderer(VBO, VAO, (GLfloat)x_parts, (GLfloat)y_parts, vertexColorLocation);

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
        ren.drawField();

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

