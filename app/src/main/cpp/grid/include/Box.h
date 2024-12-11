//
// Created by Artem on 03.12.2024.
//

#ifndef TUSA_ANDROID_BOX_H
#define TUSA_ANDROID_BOX_H

#include <stdlib.h>
#include <string>

struct Box {
public:
    int lb_x;
    int lb_y;
    int rt_x;
    int rt_y;
    uint64_t titleId;

    bool intersects(Box& box) {
        int box1_xMax = box.rt_x;
        int box2_xMin = this->lb_x;
        int box2_xMax = this->rt_x;
        int box1_xMin = box.lb_x;

        int box1_yMax = box.lb_y;
        int box2_yMin = this->rt_y;
        int box2_yMax = this->lb_y;
        int box1_yMin = box.rt_y;

        return (box1_xMax >= box2_xMin && box2_xMax >= box1_xMin &&
                box1_yMax >= box2_yMin && box2_yMax >= box1_yMin);
    }
};


#endif //TUSA_ANDROID_BOX_H