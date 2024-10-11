//
// Created by Artem on 07.10.2024.
//

#ifndef TUSA_ANDROID_MAPWIDELINE_H
#define TUSA_ANDROID_MAPWIDELINE_H

#include <vector>

class MapWideLine {
public:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> perpendiculars;
    std::vector<float> uv;
};


#endif //TUSA_ANDROID_MAPWIDELINE_H
