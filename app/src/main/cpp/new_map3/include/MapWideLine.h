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
    std::vector<float> pointsVertices;
    std::vector<float> color;
    std::vector<float> uv;
    std::vector<float> shiftVector;
};


#endif //TUSA_ANDROID_MAPWIDELINE_H
