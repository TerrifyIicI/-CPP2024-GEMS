#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

enum ShapeType {
    REULE,
    SQUARE,
    ELLIPSE,
    TRIANGLE,
    RHOMBUS
};

enum Color {
    Red,
    Green,
    Blue,
    Yellow,
    Black,
};

struct GameObject {
    int x;
    int y;
    Color color;
    ShapeType shapeType;
};

#endif // GAMEOBJECTS_H
