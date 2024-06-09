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

// Window dimensions
GLuint WIDTH = 600, HEIGHT = 800;
const int x_parts = 5, y_parts = 7;

class GameField {
private:
    bool start_flag;

    bool _removeSameColorObjects(GameObject obj1, GameObject obj2) {

        // Список объектов для удаления
        std::vector<GameObject*> objects_to_remove;
        std::vector<GameObject*> objects_to_insert;


        if (obj1.shapeType == BOMB) {
            objects_to_remove.push_back(&obj1);
            count_bomb++;
        }
        else {
            objects_to_insert = _getObjectsToRemove(obj1);
            if (!objects_to_insert.empty()) {
                objects_to_remove.push_back(&obj1);
                objects_to_remove.insert(objects_to_remove.end(), objects_to_insert.begin(), objects_to_insert.end());
            }
        }

        objects_to_insert.clear();

        if (obj2.shapeType == BOMB) {
            objects_to_remove.push_back(&obj2);
            count_bomb++;
        }
        else {
            if (obj1.color != obj2.color && obj1.shapeType == obj2.shapeType) {
                objects_to_insert = _getObjectsToRemove(obj2);
                if (!objects_to_insert.empty()) {
                    objects_to_remove.push_back(&obj2);
                    objects_to_remove.insert(objects_to_remove.end(), objects_to_insert.begin(), objects_to_insert.end());
                }
            }
        }

        _deleteGameObject(objects_to_remove);
        if (!objects_to_remove.empty()) {
            return true;
        }
        return false;
    }

    std::vector<GameObject*> _getObjectsToRemove(GameObject obj1) {
        std::vector<GameObject*> objects_to_remove;

        if (obj1.shapeType != BOMB && obj1.shapeType != FILL)
            _checkNeighbors(obj1.color, obj1.x, obj1.y, objects_to_remove, POINT);

        // Если в списке для удаления меньше 3 объектов, очищаем список
        if (objects_to_remove.size() < 2) {
            objects_to_remove.clear();
        }

        return objects_to_remove;
    }

    enum Direction {
        LEFT,
        RIGHT,
        UP,
        DOWN,
        POINT
    };

    // Метод для проверки соседних объектов и добавления их в список для удаления, если они имеют тот же цвет
    void _checkNeighbors(Color color, int x, int y, std::vector<GameObject*>& objects_to_remove, Direction dir) {
        // Проверяем правого соседа
        if (dir != RIGHT && x < rows.size() && getObject(x + 1, y).color == color) {
            GameObject* obj = &getObject(x + 1, y);
            // Проверяем, есть ли obj в objects_to_remove
            if (std::find(objects_to_remove.begin(), objects_to_remove.end(), obj) == objects_to_remove.end()) {
                if (obj->shapeType != BOMB && obj->shapeType != FILL) {
                    objects_to_remove.push_back(obj);
                    _checkNeighbors(color, x + 1, y, objects_to_remove, LEFT);
                }
            }
        }

        auto it = std::next(rows.begin(), x - 1);
        // Проверяем верхнего соседа
        if (dir != UP && y < it->size() && getObject(x, y + 1).color == color) {
            GameObject* obj = &getObject(x, y + 1);
            // Проверяем, есть ли obj в objects_to_remove
            if (std::find(objects_to_remove.begin(), objects_to_remove.end(), obj) == objects_to_remove.end()) {
                if (obj->shapeType != BOMB && obj->shapeType != FILL) {
                    objects_to_remove.push_back(obj);
                    _checkNeighbors(color, x, y + 1, objects_to_remove, DOWN);
                }
            }
        }

        // Проверяем левого соседа
        if (dir != LEFT && x > 1 && getObject(x - 1, y).color == color) {
            GameObject* obj = &getObject(x - 1, y);
            // Проверяем, есть ли obj в objects_to_remove
            if (std::find(objects_to_remove.begin(), objects_to_remove.end(), obj) == objects_to_remove.end()) {
                if (obj->shapeType != BOMB && obj->shapeType != FILL) {
                    objects_to_remove.push_back(obj);
                    _checkNeighbors(color, x - 1, y, objects_to_remove, RIGHT);
                }
            }
        }

        // Проверяем нижнего соседа
        if (dir != DOWN && 1 < y && getObject(x, y - 1).color == color) {
            GameObject* obj = &getObject(x, y - 1);
            // Проверяем, есть ли obj в objects_to_remove
            if (std::find(objects_to_remove.begin(), objects_to_remove.end(), obj) == objects_to_remove.end()) {
                if (obj->shapeType != BOMB && obj->shapeType != FILL) {
                    objects_to_remove.push_back(obj);
                    _checkNeighbors(color, x, y - 1, objects_to_remove, UP);
                }
            }
        }
    }

    void _deleteGameObject(GameObject& obj) {
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
            _updateGameObjectsX(row_it, x);

            row_it = rows.begin();
            std::advance(row_it, x - 1);
            rows.erase(row_it);
        }
        else {
            _updateGameObjectsY(*row_it, y);
        }
    }


    void _deleteGameObject(std::vector<GameObject*>& objects_to_remove) {
        if (!start_flag) {
            if (!objects_to_remove.empty()) {
                animate->animateObjectMovement(objects_to_remove);
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


        std::list<GameObject*> bonus_squares;
        if (!start_flag) {
            for (const auto& obj : objects_to_remove) {
                std::list<GameObject*> non_empty_squares = _getNonEmptySquaresInRadius(obj, 3);
                _SetRandomShapeType(non_empty_squares);
                bonus_squares.insert(bonus_squares.end(), non_empty_squares.begin(), non_empty_squares.end());
            }

            // Сортировка списка bonus_squares перед удалением повторов
            bonus_squares.sort([](const GameObject* a, const GameObject* b) {
                if (a->x == b->x) {
                    return a->y < b->y;
                }
                return a->x < b->x;
                });

            bonus_squares.erase(std::unique(bonus_squares.begin(), bonus_squares.end(),
                [](const GameObject* a, const GameObject* b) {
                    return a->x == b->x && a->y == b->y;
                }), bonus_squares.end());

            // Удаление всех вхождений objects_to_remove из bonus_squares
            bonus_squares.remove_if([&objects_to_remove](const GameObject* obj) {
                return std::find(objects_to_remove.begin(), objects_to_remove.end(), obj) != objects_to_remove.end();
                });
        }

        for (const auto& obj : objects_to_remove) {
            _deleteGameObject(*obj);
        }

    }

    void _SetRandomShapeType(std::list<GameObject*>& non_empty_squares) {
        // Проверяем, что список не пуст
        if (!non_empty_squares.empty()) {
            // Инициализируем генератор случайных чисел один раз
            static bool initialized = false;
            if (!initialized) {
                srand(time(0));
                initialized = true;
            }

            // Получаем случайный индекс
            int random_index = rand() % non_empty_squares.size();

            // Находим элемент по случайному индексу
            auto it = non_empty_squares.begin();
            std::advance(it, random_index);

            // Выбираем случайный shapeType: FILL или BOMB
            int random_choice = rand() % 2; // 0 или 1
            if ((*it)->shapeType != BOMB && (*it)->shapeType != FILL) {
                if (random_choice == 0) {
                    (*it)->shapeType = FILL;
                }
                else {
                    (*it)->shapeType = BOMB;
                }
            }
        }
    }

    void _updateGameObjectsY(std::list<GameObject>& row, int start_y) {
        for (auto& game_obj : row) {
            if (game_obj.y > start_y) {
                game_obj.y--;
            }
        }
    }

    void _updateGameObjectsX(std::list<std::list<GameObject>>::iterator& row, int start_x) {
        while (row != rows.end()) {
            for (auto& game_obj : *row) {
                game_obj.x--;
            }
            ++row;
        }
    }

    std::list<GameObject*> _getNonEmptySquaresInRadius(GameObject* obj, int radius) {
        std::list<GameObject*> non_empty_squares;
        auto row = rows.begin();
        for (int i = 0; i < obj->x - radius && i < rows.size(); i++) {
            ++row;
        }

        auto row_end = rows.begin();
        std::advance(row_end, rows.size() - 1);

        std::vector<std::pair<int, int>> potential_points;
        for (int i = -radius; i <= radius; ++i) {
            auto& rowList = *row;
            if (obj->x + i >= 1 && obj->x + i <= rows.size()) {
                for (int j = -radius; j <= radius; ++j) {
                    if ((i * i + j * j <= radius * radius) && !(j == 0 && i == 0) &&
                        (obj->y + j <= rowList.size()) && (obj->y + j >= 1)) {
                        non_empty_squares.push_back(&getObject(obj->x + i, obj->y + j));
                    }
                }
                if (row == row_end) {
                    potential_points.clear();
                    return non_empty_squares;
                }
                ++row;
            }
        }
        potential_points.clear();
        return non_empty_squares;
    }

    void _shuffle(std::vector<std::pair<int, int>>& vec) {
        srand(time(0));
        for (std::size_t i = vec.size() - 1; i > 0; --i) {
            std::size_t j = std::rand() % (i + 1);
            std::swap(vec[i], vec[j]);
        }
    }

    void _Fill(GameObject* obj) {
        fill_flag = true;
    }
public:
    GameField(int x, int y) {
        x_parts = x;
        y_parts = y;
        GenerateField(SQUARE);
    }

    void GenerateField(ShapeType type) {
        start_flag = true;
        srand(time(0));
        int colors = std::ranges::min(x_parts, y_parts) - std::ranges::min(x_parts, y_parts) / 3;
        do {
            while (rows.size() < x_parts) {
                rows.push_back({});
            }
            for (auto it = rows.begin(); it != rows.end(); ++it) {
                int x = std::distance(rows.begin(), it) + 1;
                while (it->size() < y_parts) {
                    it->push_back({ x, (int)it->size() + 1, static_cast<Color>(rand() % colors), type });
                }
            }
        } while (!removeSameColorObjects());
        start_flag = false;
    }

    // Метод для получения объекта по заданным координатам
    GameObject& getObject(int x, int y) {

        static GameObject defaultObject{ 0, 0, WHITE, BOMB };
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

    bool swapObjects(int x_index, int y_index, int x_index_last, int y_index_last) {
        auto& obj1 = getObject(x_index, y_index);
        auto& obj2 = getObject(x_index_last, y_index_last);

        if (animate != nullptr) {
            auto obj1_copy = obj1;
            auto obj2_copy = obj2;
            animate->swapObject(obj1_copy, obj2_copy);
        }


        std::swap(obj1.color, obj2.color);
        std::swap(obj1.shapeType, obj2.shapeType);


        _removeSameColorObjects(obj1, obj2);
        return true;
    }

    std::vector<GameObject*> _getRandomObject(int count) {
        std::vector<GameObject*> objects_to_remove;
        // Собираем все объекты в один вектор для удобного доступа
        std::vector<std::pair<int, int>> all_objects;

        int i = 0;
        for (auto& row : rows) {
            i++;
            for (int j = 1; j <= row.size(); j++) {
                all_objects.push_back(std::make_pair(i, j));
            }
        }

        // Перемешиваем вектор объектов
        if (!all_objects.empty()) {
            _shuffle(all_objects);
        }

        // Добавляем произвольные объекты (но не больше чем размер all_objects)
        for (size_t i = 0; i < count && i < all_objects.size(); ++i) {
            objects_to_remove.push_back(&getObject(all_objects[i].first, all_objects[i].second));
            if (objects_to_remove[i]->shapeType == BOMB) {
                count_bomb++;
            }
        }

        // Удаляем объекты
        return objects_to_remove;
    }

    bool hasThreeOfSameColor() {
        for (const auto& row : rows) {
            // Используем карту для подсчета количества объектов каждого цвета в текущем ряду
            std::map<Color, int> colorCount;
            for (const auto& obj : row) {
                colorCount[obj.color]++;
                if (colorCount[obj.color] == 2) {
                    // Найдено 3 объекта одного цвета, возвращаем true
                    return true;
                }
            }
        }
        // Не найдено 3 объекта одного цвета, возвращаем false
        return false;
    }

    std::list<std::list<GameObject>> rows;
    inline static int x_parts;
    inline static int y_parts;
    inline static int x_index, y_index;
    inline static int x_index_last, y_index_last;
    std::unique_ptr<IRenderer> animate;

    bool fill_flag;
    int count_bomb;

    bool removeSameColorObjects() {
        std::vector<GameObject*> objects_to_remove;

        // Проходим по всем строкам
        for (auto& row : rows) {
            // Проходим по всем объектам в строке
            for (auto& obj : row) {
                // Проверяем, есть ли объект уже в списке объектов для удаления
                if (std::find(objects_to_remove.begin(), objects_to_remove.end(), &obj) == objects_to_remove.end()) {
                    // Получаем список объектов для удаления
                    std::vector<GameObject*> to_remove = _getObjectsToRemove(obj);

                    // Если количество объектов для удаления больше 3
                    if (to_remove.size() >= 2) {
                        // Добавляем объект в список объектов для удаления
                        objects_to_remove.push_back(&obj);

                        // Добавляем объекты для удаления из to_remove в objects_to_remove
                        objects_to_remove.insert(objects_to_remove.end(), to_remove.begin(), to_remove.end());
                    }
                }
            }
        }

        // Удаляем объекты
        _deleteGameObject(objects_to_remove);

        // Возвращаем true, если были удалены объекты
        return objects_to_remove.empty();
    }

    void Update() {
        if (count_bomb > 0) {
            count_bomb--;
            std::vector<GameObject*> objects_to_remove = _getRandomObject(4);
            _deleteGameObject(objects_to_remove);
        }
        else if (hasThreeOfSameColor()) {
            removeSameColorObjects();
        }
    }
};


class GameRenderer : public Renderer, public GameField, public IRenderer {
private:
    inline static bool _click_on_fild;

    void _displayFieldClick() {
        if (_click_on_fild)
            drawSquareLines(x_index, y_index);
    }

    void _FieldClick() {
        // проверяем, что текущий квадрат находится в диапазоне от (1,1) до (x_parts, x_parts)
        if (x_index >= 1 && x_index <= x_parts && y_index >= 1 && y_index <= y_parts) {
            _click_on_fild = true;
            if (((abs(x_index - x_index_last) < 1 && abs(y_index - y_index_last) <= 1) ||
                (abs(x_index - x_index_last) <= 1 && abs(y_index - y_index_last) < 1))) {
                interface->swapObjects(x_index, y_index, x_index_last, y_index_last);
                x_index_last = 0; y_index_last = 0;
            }
            else {
                x_index_last = x_index;
                y_index_last = y_index;
            }
        }
        else
            _click_on_fild = false;
    }

    inline static GameRenderer* interface;

    void _updateOldField() {
        std::vector<GameObject*> objects_to_remove;
        if (!rows.empty()) {
            for (auto& data : _move_queue) {
                if (data.y_new > 0 && data.x_new > 0 && data.x_new < rows.size()) {
                    GameObject& new_obj = getObject(data.x_new, data.y_new);
                    if (&new_obj != nullptr && &new_obj != &data.obj && new_obj.y > 0) {
                        objects_to_remove.push_back(&new_obj);
                    }
                }
            }
        }
        _remove_objects(objects_to_remove);
    }

    struct SwapObjectData {
        GameObject obj1;
        GameObject obj2;
    };

    struct AnimateObjectMovementData {
        GameObject obj;
        int x_new;
        int y_new;
    };

    using AnimeteSwap = std::vector<SwapObjectData>;
    using AnimeteMovemen = std::vector<AnimateObjectMovementData>;
    std::list<std::list<GameObject>> _rows_copy;

    AnimeteSwap _swap_queue;
    AnimeteMovemen _move_queue;

    void _remove_objects(std::vector<GameObject*>& objects_to_remove) {
        _rows_copy = rows;
        for (auto obj_ptr : objects_to_remove) {
            int x = obj_ptr->x;
            int y = obj_ptr->y;

            // Ищем объект в списке _rows_copy
            auto rowIter = std::next(_rows_copy.begin(), x - 1);
            auto objIter = std::find_if(rowIter->begin(), rowIter->end(), [&](const GameObject& obj_copy) {
                return obj_copy.x == x && obj_copy.y == y;
                });

            // Если объект найден в списке _rows_copy, удаляем его
            if (objIter != rowIter->end()) {
                objIter = rowIter->erase(objIter);
            }
        }
    }


public:
    GameRenderer(GLuint VBO_, GLuint VAO_, int x_parts, int y_parts, GLint vertexColorLocation_)
        : Renderer(VBO_, VAO_, x_parts, y_parts, vertexColorLocation_), GameField(x_parts, y_parts) {
        interface = this;
        animate.reset(this);
    }

    void drawField() {
        for (auto& row : rows) {
            for (const auto& obj : row) {
                drawObject(obj);
            }
        }
    }

    void drawOldField() {
        for (auto& row : _rows_copy) {
            for (const auto& obj : row) {
                drawObject(obj);
            }
        }
    }

    void drawSwap() {
        for (auto& data : _swap_queue) {
            Animate(data.obj1, data.obj2, true);
        }
        if (animate_step == steps || animate_step == 0) {
            _swap_queue.clear();
            Update();
        }
    }

    void drawMove() {
        if (animate_step == steps || animate_step == 0) {
            _updateOldField();
        }
        for (auto& data : _move_queue) {
            Animate(data.obj, data.x_new, data.y_new, true);
        }
        animate_step -= _move_queue.size() - 2;
        if (animate_step >= steps - _move_queue.size() || animate_step <= -2) {
            _move_queue.clear();
            Update();
            animate_step = 0;
        }
    }

    void GEMS() {

        if (!_swap_queue.empty()) {
            drawOldField();
            drawSwap();
        }
        else if (!_move_queue.empty())
        {
            drawOldField();
            drawMove();
        }
        else {
            drawField();
        }
        glUniform4f(vertexColorLocation, 0.0f, 0.0f, 0.0f, 1.0f); //черный
        drawBorderLines();
        drawGrid(x_parts, y_parts);

        _displayFieldClick();
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


            interface->_FieldClick();

        }
    }

    void swapObject(GameObject& obj1, GameObject& obj2) {
        _swap_queue.push_back({ obj1, obj2 });
        std::vector<GameObject*> objects_to_remove{ &obj1, &obj2 };
        _remove_objects(objects_to_remove);
    }

    void animateObjectMovement(std::vector<GameObject*>& objects_to_remove) {

        if (objects_to_remove.size() == 1) {
            return;
        }

        std::sort(objects_to_remove.begin(), objects_to_remove.end(),
            [](const GameObject* a, const GameObject* b) {
                if (a->x == b->x) {
                    return a->y < b->y;
                }
                else {
                    return a->x < b->x;
                }
            });

        int deltaX = 0;
        auto row = rows.begin();
        auto remove_obj = objects_to_remove.begin();
        auto remove_end = objects_to_remove.begin();
        std::advance(remove_end, objects_to_remove.size() - 1);

        GameObject* remove = *remove_obj;
        while (true) {
            int deltaY = 0;
            for (auto& game_obj : *row) {
                if (remove_obj != remove_end && *remove == game_obj) {
                    deltaY++;
                    ++remove_obj;
                    remove = *remove_obj;
                }
                else if ((deltaY != 0 || deltaX != 0) && (game_obj.x - deltaX > 0 || game_obj.y - deltaY > 0)) {
                    _move_queue.push_back({ game_obj, game_obj.x - deltaX, game_obj.y - deltaY });
                }
                if (deltaY == std::distance(row->begin(), row->end())) {
                    deltaX++;
                }
                else if (remove_obj == remove_end) {
                    return;
                }
            }
            if (remove_obj != objects_to_remove.end()) {
                while (remove->x - 1 > std::distance(rows.begin(), row)) {
                    ++row;
                }
            }
            else
                return;
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
        ren.GEMS();

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

