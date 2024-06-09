#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

enum ShapeType
{
    REULE,
    SQUARE,
    ELLIPSE,
    TRIANGLE,
    RHOMBUS,
    BOMB
};

enum Color {
    Red,
    Green,
    Blue,
    Yellow,
    Cyan,
    Magenta,
    Brown,
    Gray,
    Pink,
    Orange,
    Black,
    White
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
