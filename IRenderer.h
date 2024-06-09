#ifndef IRENDERER_H
#define IRENDERER_H

#include "GameObjects.h"

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void swapObject(GameObject& obj1, GameObject& obj2) = 0;
    virtual void animateObjectMovement(GameObject& obj, int x_new, int y_new) = 0;
    // �������� ������ ������ ���������� � ��������, ������� ��� �����
};

#endif // IRENDERER_H

