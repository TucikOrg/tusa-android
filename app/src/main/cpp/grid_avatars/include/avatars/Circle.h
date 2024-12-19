//
// Created by Artem on 19.12.2024.
//

#ifndef TUSA_ANDROID_AVATARS_CIRCLE_H
#define TUSA_ANDROID_AVATARS_CIRCLE_H

#include <stdint.h>
#include <math.h>
#include "IGridForm.h"

namespace Avatars {
    struct Circle {
    public:
        Circle() {}
        Circle(int x, int y, int radius, int64_t id)
            : x(x), y(y), radius(radius), id(id) {}

        int x;
        int y;
        int radius;
        int64_t id;

        bool intersects(Circle& circle, int& dx, int&dy, int& length) {
            dx = x - circle.x;
            dy = y - circle.y;
            int distance = sqrt(dx * dx + dy * dy);
            int radiusSum = radius + circle.radius;
            length = distance - radiusSum;
            return distance <= radiusSum;
        }
    };
}



#endif //TUSA_ANDROID_CIRCLE_H
