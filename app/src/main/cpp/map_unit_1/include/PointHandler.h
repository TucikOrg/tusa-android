//
// Created by Artem on 09.11.2024.
//

#ifndef TUSA_ANDROID_POINTHANDLER_H
#define TUSA_ANDROID_POINTHANDLER_H


#include <stdint.h>
#include <vector>
#include <geometry.hpp>

class PointHandler {
public:
    std::vector<vtzero::point> points = {};
    uint index = 0;

    void points_begin(uint32_t count) {
        points = std::vector<vtzero::point>(count);
    }

    void points_point(vtzero::point point) {
        points[index] = point;
        index++;
    }

    void points_end() {

    }
};


#endif //TUSA_ANDROID_POINTHANDLER_H
