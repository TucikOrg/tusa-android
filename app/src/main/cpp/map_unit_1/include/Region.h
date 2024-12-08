//
// Created by Artem on 07.12.2024.
//

#ifndef TUSA_ANDROID_REGION_H
#define TUSA_ANDROID_REGION_H

#include <vector_tile.hpp>
#include "PointForTextPathCache.h"

struct Region {
    short type;
    std::vector<vtzero::point> points;
    std::string name;
    uint64_t pathIndex;

    bool intersectsMe(Region& compareWith, float delta) {
        int count = 0;
        auto& regionPoints = points;
        auto& otherPoints = compareWith.points;
        for (int i = 0; i < regionPoints.size(); i++) {
            auto& point = regionPoints[i];

            for (int i2 = 0; i2 < otherPoints.size(); i2++) {
                auto& other = otherPoints[i2];

                float len = sqrt( pow(point.x - other.x, 2.0) + pow(-point.y + other.y, 2.0) );
                bool intersects = len < delta;
                if (intersects) {
                    count++;
                    return true;
                }
            }

        }
        return false;
    }
};


#endif //TUSA_ANDROID_REGION_H
