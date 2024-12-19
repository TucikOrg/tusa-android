//
// Created by Artem on 19.12.2024.
//

#ifndef TUSA_ANDROID_CIRCLE_H
#define TUSA_ANDROID_CIRCLE_H

#include "IGridForm.h"

struct Circle: public IGridForm {
public:
    int x;
    int y;
    int radius;

    bool intersects(IGridForm& form) const override {
        Circle& circle = static_cast<Circle&>(form);
        int dx = x - circle.x;
        int dy = y - circle.y;
        int distance = dx * dx + dy * dy;
        int radiusSum = radius + circle.radius;
        return distance <= radiusSum * radiusSum;
    }
};


#endif //TUSA_ANDROID_CIRCLE_H
