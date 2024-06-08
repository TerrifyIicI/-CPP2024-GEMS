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
#include <algorithm>
#include "Renderer.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Key callback function
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// Window dimensions
GLuint WIDTH = 600, HEIGHT = 800;
const int x_parts = 5, y_parts = 7;

class GameField {
public:
    GameField(int x, int y) {
        x_parts = x;
        y_parts = y;
        GenerateField(SQUARE);
    }

    void GenerateField(ShapeType type) {
        srand(time(0));
        for (int i = 0; i < x_parts; ++i) {
            rows.push_back(std::list<GameObject>());
            for (int j = 1; j <= y_parts; ++j) {
                rows[i].push_back({ i + 1, j, static_cast<Color>(rand() % 3), type });
            }
        }
    }

    bool swapObjects(int x_index, int y_index, int x_index_last, int y_index_last) {
        // Find the objects to swap
        auto& obj1 = *std::find_if(rows[x_index - 1].begin(), rows[x_index - 1].end(), [&](const GameObject& obj) {
            return obj.x == x_index && obj.y == y_index;
            });

        auto& obj2 = *std::find_if(rows[x_index_last - 1].begin(), rows[x_index_last - 1].end(), [&](const GameObject& obj) {
            return obj.x == x_index_last && obj.y == y_index_last;
            });

        // Swap the color and shapeType of the objects
        std::swap(obj1.color, obj2.color);
        std::swap(obj1.shapeType, obj2.shapeType);

        // Return true
        return true;
    }

    std::vector<std::list<GameObject>> rows;
    inline static int x_parts;
    inline static int y_parts;
    inline static int x_index, y_index;
    inline static int x_index_last, y_index_last;
};

class GameRenderer : public Renderer, public GameField {
private:
    inline static GameRenderer* interface;
public:
    GameRenderer(GLuint VBO_, GLuint VAO_, int x_parts, int y_parts, GLint vertexColorLocation_)
        : Renderer(VBO_, VAO_, x_parts, y_parts, vertexColorLocation_), GameField(x_parts, y_parts) {
        interface = this;
    }

    void drawField() {
        for (auto& row : rows) {
            for (const auto& obj : row) {
                drawObject(obj);
            }
        }

        glUniform4f(vertexColorLocation, 0.0f, 0.0f, 0.0f, 1.0f); // черный
        drawBorderLines();
        drawGrid(x_parts, y_parts);
        displayFieldClick();
    }

    // обработчик события нажатия мыши
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            // получаем текущие координаты курсора мыши
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            // определяем, в каком квадрате сетки находится курсор мыши
            GameRenderer::x_index = static_cast<int>(xpos / (WIDTH / (x_parts + 2)));
            GameRenderer::y_index = static_cast<int>(ypos / (HEIGHT / (y_parts + 3)));
            GameRenderer::y_index = y_parts + 2 - GameRenderer::y_index;

            // проверяем, что текущий квадрат находится в диапазоне от (1,1) до (x_parts, x_parts)
            if (GameRenderer::x_index >= 1 && GameRenderer::x_index <= x_parts && GameRenderer::y_index >= 1 && GameRenderer::y_index <= y_parts) {
                click_on_fild = true;
                interface->FieldClick();
            }
            else
                click_on_fild = false;
        }
    }
    inline static bool click_on_fild;

    void displayFieldClick() {
        if (click_on_fild)
            drawSquareLines(GameRenderer::x_index, GameRenderer::y_index);
    }

    void FieldClick() {
        if ((((abs(GameRenderer::x_index - GameRenderer::x_index_last) < 1 && abs(GameRenderer::y_index - GameRenderer::y_index_last) <= 1) ||
            (abs(GameRenderer::x_index - GameRenderer::x_index_last) <= 1 && abs(GameRenderer::y_index - GameRenderer::y_index_last) < 1)) &&
            (GameRenderer::x_index != GameRenderer::x_index_last || GameRenderer::y_index != GameRenderer::y_index_last))) {
            interface->swapObjects(GameRenderer::x_index, GameRenderer::y_index, GameRenderer::x_index_last, GameRenderer::y_index_last);
            GameRenderer::x_index_last = 0; GameRenderer::y_index_last = 0;
        }
        else {
            GameRenderer::x_index_last = GameRenderer::x_index;
            GameRenderer::y_index_last = GameRenderer::y_index;
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

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, ren.mouse_button_callback);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the field
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
