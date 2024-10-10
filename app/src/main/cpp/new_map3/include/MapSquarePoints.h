//
// Created by Artem on 10.10.2024.
//

#ifndef TUSA_ANDROID_MAPSQUAREPOINTS_H
#define TUSA_ANDROID_MAPSQUAREPOINTS_H

#include <vector>

class MapSquarePoints {
public:
    std::vector<float> vertices;
    std::vector<float> uvs;
    std::vector<unsigned int> indices;
    std::vector<float> shifts;
};


#endif //TUSA_ANDROID_MAPSQUAREPOINTS_H
