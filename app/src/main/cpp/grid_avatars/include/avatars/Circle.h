//
// Created by Artem on 19.12.2024.
//

#ifndef TUSA_ANDROID_AVATARS_CIRCLE_H
#define TUSA_ANDROID_AVATARS_CIRCLE_H

#include <stdint.h>
#include <math.h>
#include "IGridForm.h"
#include "avatars/AvatarIntersection.h"
#include <vector>
#include <unordered_map>

namespace Avatars {
    struct Circle {
    public:
        Circle() {}
        Circle(int x, int y, int radius, int64_t id)
            : x(x), y(y), radius(radius), id(id), realX(x), realY(y) {}

        int x;
        int y;
        int radius;
        int64_t id;

        int realX;
        int realY;

        bool intersects(Circle& circle, float& dx, float&dy, float& length) {
            dx = circle.realX - realX;
            dy = circle.realY - realY;
            float distance = sqrt(dx * dx + dy * dy);
            float radiusSum = radius + circle.radius;
            length = radiusSum - distance;
            dx /= distance;
            dy /= distance;
            return distance < radiusSum;
        }

        std::vector<AvatarIntersection> findIntersections(
                std::vector<Circle>& circles,
                std::unordered_map<int64_t, void*>& ignoreMeInCollisionsChecks
        ) {
            std::vector<AvatarIntersection> intersections = {};
            for (auto& otherCircle : circles) {
                if (ignoreMeInCollisionsChecks.count(otherCircle.id) > 0) continue;

                float dx,dy,len;
                auto intersects = this->intersects(otherCircle, dx, dy, len);
                if (!intersects) continue;

                intersections.push_back(AvatarIntersection {
                        dx, dy, len,
                        otherCircle.id,
                });
            }
            return intersections;
        }
    };
}



#endif //TUSA_ANDROID_CIRCLE_H
