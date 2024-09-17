//
// Created by Artem on 16.09.2024.
//

#ifndef TUSA_ANDROID_MAPGEOMETRY_H
#define TUSA_ANDROID_MAPGEOMETRY_H

#include <vector>

class MapGeometry {
public:
    std::vector<float> generatePlainGeometry(
            float size,
            int segments,
            float radius,
            std::vector<float>& uv,
            std::vector<unsigned int>& indices
    );
};


#endif //TUSA_ANDROID_MAPGEOMETRY_H
