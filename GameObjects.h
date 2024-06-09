#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

enum ShapeType
{
    REULE,
    SQUARE,
    ELLIPSE,
    TRIANGLE,
    RHOMBUS,
    BOMB,
    FILL
};

enum Color
{
    RED,
    GREEN,
    BLUE,
    YELLOW,
    MAGENTA,
    CYAN,
    BROWN,
    GRAY,
    PINK,
    ORANGE,
    BLACK,
    WHITE
};

struct GameObject {
    int x;
    int y;
    Color color;
    ShapeType shapeType;
    bool operator==(const GameObject& other) const
    {
        return (x == other.x && y == other.y);
    }
};

#endif // GAMEOBJECTS_H
