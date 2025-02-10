//
// Created by Artem on 10.02.2025.
//

#ifndef TUSA_ANDROID_ROADLETTERSPTR_H
#define TUSA_ANDROID_ROADLETTERSPTR_H

#include <stdlib.h>

struct RoadLettersPtr {
    uint64_t roadId;
    uint64_t tileKey;
    size_t startIndex;
    size_t amount;
    short regionId;
};


#endif //TUSA_ANDROID_ROADLETTERSPTR_H
