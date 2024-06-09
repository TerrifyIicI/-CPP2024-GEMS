#include <iostream>
#define _USE_MATH_DEFINES
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <math.h>
#include <string>
#include <list>
#include <vector>
#include <ranges>
#include <functional>
#include <variant>
#include <algorithm>
#include "Renderer.h"
#include "Shader.h"
#include <set>
#include <map>

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

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
            std::list<GameObject> newRow;
            for (int j = 1; j <= y_parts; ++j) {
                newRow.push_back({ i + 1, j, static_cast<Color>(rand() % 3), type });
            }
            rows.push_back(newRow);
        }
    }


    bool swapObjects(int x_index, int y_index, int x_index_last, int y_index_last) {
        auto& obj1 = getObject(x_index, y_index);
        auto& obj2 = getObject(x_index_last, y_index_last);

        std::swap(obj1.color, obj2.color);
        std::swap(obj1.shapeType, obj2.shapeType);

        removeSameColorObjects(obj1, obj2);
        return true;
    }

    enum Direction {
        LEFT,
        RIGHT,
        UP,
        DOWN,
        POINT
    };

    bool removeSameColorObjects(GameObject obj1, GameObject obj2) {

        // Список объектов для удаления
        std::vector<GameObject*> objects_to_remove;


        std::vector<GameObject*> objects_to_insert = getObjectsToRemove(obj1);
        if (!objects_to_insert.empty()) {
            objects_to_remove.push_back(&obj1);
            objects_to_remove.insert(objects_to_remove.end(), objects_to_insert.begin(), objects_to_insert.end());
        }

        objects_to_insert.clear();

        if (obj1.color != obj2.color) {
            objects_to_insert = getObjectsToRemove(obj2);
            if (!objects_to_insert.empty()) {
                objects_to_remove.push_back(&obj2);
                objects_to_remove.insert(objects_to_remove.end(), objects_to_insert.begin(), objects_to_insert.end());
            }
        }

        std::sort(objects_to_remove.begin(), objects_to_remove.end(),
            [](const GameObject* a, const GameObject* b) {
                if (a->x == b->x) {
                    return a->y > b->y;
                }
                else {
                    return a->x > b->x;
                }
            });

        objects_to_remove.erase(std::unique(objects_to_remove.begin(), objects_to_remove.end(),
            [](const GameObject* a, const GameObject* b) {
                return a->x == b->x && a->y == b->y;
            }), objects_to_remove.end());

        for (const auto& obj : objects_to_remove) {
            deleteGameObject(*obj);
        }
        return false;
    }

    std::vector<GameObject*> getObjectsToRemove(GameObject obj1) {
        std::vector<GameObject*> objects_to_remove;

        checkNeighbors(obj1.color, obj1.x, obj1.y, objects_to_remove, POINT);

        // Если в списке для удаления меньше 3 объектов, очищаем список
        if (objects_to_remove.size() < 2) {
            objects_to_remove.clear();
        }

        return objects_to_remove;
    }


    // Метод для проверки соседних объектов и добавления их в список для удаления, если они имеют тот же цвет
    void checkNeighbors(Color color, int x, int y, std::vector<GameObject*>& objects_to_remove, Direction dir) {
        // Проверяем правого соседа
        if (dir != RIGHT && x < rows.size() && getObject(x + 1, y).color == color) {
            GameObject* obj = &getObject(x + 1, y);
            // Проверяем, есть ли obj в objects_to_remove
            if (std::find(objects_to_remove.begin(), objects_to_remove.end(), obj) == objects_to_remove.end()) {
                // Если obj не найден, добавляем его
                objects_to_remove.push_back(obj);
                checkNeighbors(color, x + 1, y, objects_to_remove, LEFT);
            }
        }

        auto it = std::next(rows.begin(), x - 1);
        // Проверяем верхнего соседа
        if (dir != UP && y < it->size() && getObject(x, y + 1).color == color) {
            GameObject* obj = &getObject(x, y + 1);
            // Проверяем, есть ли obj в objects_to_remove
            if (std::find(objects_to_remove.begin(), objects_to_remove.end(), obj) == objects_to_remove.end()) {
                // Если obj не найден, добавляем его
                objects_to_remove.push_back(obj);
                checkNeighbors(color, x, y + 1, objects_to_remove, DOWN);
            }
        }

        // Проверяем левого соседа
        if (dir != LEFT && x > 1 && getObject(x - 1, y).color == color) {
            GameObject* obj = &getObject(x - 1, y);
            // Проверяем, есть ли obj в objects_to_remove
            if (std::find(objects_to_remove.begin(), objects_to_remove.end(), obj) == objects_to_remove.end()) {
                // Если obj не найден, добавляем его
                objects_to_remove.push_back(obj);
                checkNeighbors(color, x - 1, y, objects_to_remove, RIGHT);
            }
        }

        // Проверяем нижнего соседа
        if (dir != DOWN && 1 < y && getObject(x, y - 1).color == color) {
            GameObject* obj = &getObject(x, y - 1);
            // Проверяем, есть ли obj в objects_to_remove
            if (std::find(objects_to_remove.begin(), objects_to_remove.end(), obj) == objects_to_remove.end()) {
                // Если obj не найден, добавляем его
                objects_to_remove.push_back(obj);
                checkNeighbors(color, x, y - 1, objects_to_remove, UP);
            }
        }
    }

    // Метод для получения объекта по заданным координатам
    GameObject& getObject(int x, int y) {

        static GameObject defaultObject{ 0, 0, Black, RHOMBUS };
        if (x < 1 || y < 1 || x>rows.size()) {
            return defaultObject;
        }
        auto rowIter = std::next(rows.begin(), x - 1);
        if (rowIter->size() < y) {
            return defaultObject;
        }
        auto objIter = std::find_if(rowIter->begin(), rowIter->end(), [&](const GameObject& obj) {
            return obj.x == x && obj.y == y;
            });
        return *objIter;
    }

    void deleteGameObject(GameObject& obj) {
        // Найти нужный список (строку)
        auto row_it = rows.begin();
        std::advance(row_it, obj.x - 1);
        if (row_it == rows.end()) {
            return;
        }

        // Найти нужный объект в списке (строке)
        auto obj_it = row_it->begin();
        std::advance(obj_it, obj.y - 1);
        if (obj_it == row_it->end()) {
            return;
        }

        int x = obj.x;
        int y = obj.y;

        row_it->erase(obj_it); // Удаляем объект из строки

        if (row_it->empty()) {
            updateGameObjectsX(row_it, x);

            row_it = rows.begin();
            std::advance(row_it, x - 1);
            rows.erase(row_it);
        }
        else {
            updateGameObjectsY(*row_it, y);
        }
    }


    void updateGameObjectsY(std::list<GameObject>& row, int start_y) {
        for (auto& game_obj : row) {
            if (game_obj.y > start_y) {
                game_obj.y--;
            }
        }
    }

    void updateGameObjectsX(std::list<std::list<GameObject>>::iterator& row, int start_x) {
        while (row != rows.end()) {
            for (auto& game_obj : *row) {
                game_obj.x--;
            }
            ++row;
        }
    }


    std::list<std::list<GameObject>> rows;
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

        glUniform4f(vertexColorLocation, 0.0f, 0.0f, 0.0f, 1.0f); //черный
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
            x_index = (int)(xpos / (WIDTH / (x_parts + 2)));
            y_index = (int)(ypos / (HEIGHT / (y_parts + 3)));
            y_index = y_parts + 2 - y_index;

            // проверяем, что текущий квадрат находится в диапазоне от (1,1) до (x_parts, x_parts)
            if (x_index >= 1 && x_index <= x_parts && y_index >= 1 && y_index <= y_parts) {
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
            drawSquareLines(x_index, y_index);
    }

    void FieldClick() {
        if ((((abs(x_index - x_index_last) < 1 && abs(y_index - y_index_last) <= 1) ||
            (abs(x_index - x_index_last) <= 1 && abs(y_index - y_index_last) < 1)) &&
            (x_index != x_index_last || y_index != y_index_last))) {
            interface->swapObjects(x_index, y_index, x_index_last, y_index_last);
            x_index_last = 0; y_index_last = 0;
        }
        else {
            x_index_last = x_index;
            y_index_last = y_index;
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
