//
// Created by Artem on 05.12.2024.
//

#ifndef TUSA_ANDROID_RESERVEDLIST_H
#define TUSA_ANDROID_RESERVEDLIST_H

#include <vector>

template <typename T>
class ReservedList {
public:
    ReservedList(uint32_t reserve) {
        list = std::vector<T>(reserve);
    }

    void add(T element) {
        if (index >= list.size()) {
            list.push_back(element);
            return;
        }
        list[index++] = element;
    }

    uint32_t index = 0;
    std::vector<T> list = {};
};


#endif //TUSA_ANDROID_RESERVEDLIST_H
