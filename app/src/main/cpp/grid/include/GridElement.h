//
// Created by Artem on 03.12.2024.
//

#ifndef TUSA_ANDROID_GRIDELEMENT_H
#define TUSA_ANDROID_GRIDELEMENT_H

#include "Box.h"

struct GridElement {
public:
    uint16_t next = 0;
    uint16_t forToBoxId;
};


#endif //TUSA_ANDROID_GRIDELEMENT_H
