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
        Circle(int x, int y, int radius, int64_t id, float toWorldK)
            : x(x), y(y), radius(radius), id(id), toWorldK(toWorldK), realX(x), realY(y) {}

        int x;
        int y;
        int radius;
        int64_t id;
        float toWorldK;

        int realX;
        int realY;
        bool isRadialGrouped = false;

        bool intersects(Circle& circle, float& dx, float&dy, float& length) {
            dx = realX - circle.realX;
            dy = realY - circle.realY;
            int distance = sqrt(dx * dx + dy * dy);
            int radiusSum = radius + circle.radius;
            length = abs(distance - radiusSum);
            dx /= distance;
            dy /= distance;
            dy *= -1;
            return distance < radiusSum;
        }
    };
}



#endif //TUSA_ANDROID_CIRCLE_H
