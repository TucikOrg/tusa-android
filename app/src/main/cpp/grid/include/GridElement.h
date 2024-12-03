//
// Created by Artem on 03.12.2024.
//

#ifndef TUSA_ANDROID_GRIDELEMENT_H
#define TUSA_ANDROID_GRIDELEMENT_H

#include "Box.h"

struct GridElement {
public:
    int next = -1;
    uint64_t boxId;
};


#endif //TUSA_ANDROID_GRIDELEMENT_H
