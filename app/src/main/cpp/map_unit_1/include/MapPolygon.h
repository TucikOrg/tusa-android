//
// Created by Artem on 16.09.2024.
//

#ifndef TUSA_ANDROID_MAPPOLYGON_H
#define TUSA_ANDROID_MAPPOLYGON_H

#include <vector>

class MapPolygon {
public:
    std::vector<std::vector<std::array<float, 2>>> points;
    std::vector<unsigned int> indices;
};


#endif //TUSA_ANDROID_MAPPOLYGON_H
