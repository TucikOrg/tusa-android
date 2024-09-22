//
// Created by Artem on 16.09.2024.
//

#ifndef TUSA_ANDROID_MAPGEOMETRY_H
#define TUSA_ANDROID_MAPGEOMETRY_H

#include <vector>

class MapGeometry {
public:
    std::vector<float> generatePlainGeometry(
            std::vector<float>& uv,
            std::vector<float>& planetUV,
            std::vector<unsigned int>& indices,
            std::vector<int> visibleY,
            int z,
            float planeWidth,
            int lod,
            float planeZ
    );

};


#endif //TUSA_ANDROID_MAPGEOMETRY_H
