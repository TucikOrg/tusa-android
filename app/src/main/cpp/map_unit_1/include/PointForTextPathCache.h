//
// Created by Artem on 07.12.2024.
//

#ifndef TUSA_ANDROID_POINTFORTEXTPATHCACHE_H
#define TUSA_ANDROID_POINTFORTEXTPATHCACHE_H

#include <vector_tile.hpp>

struct PointForTextPathCache {
public:
    vtzero::point point;
    uint64_t pathIndex;
    std::string name;

    bool intersects(vtzero::point other) {
        float len = sqrt( pow(point.x - other.x, 2.0) + pow(point.y - other.y, 2.0) );
        return len < 150;
    }
};


#endif //TUSA_ANDROID_POINTFORTEXTPATHCACHE_H
