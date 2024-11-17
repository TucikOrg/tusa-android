//
// Created by Artem on 08.10.2024.
//

#ifndef TUSA_ANDROID_MAPSIMPLEPOINT_H
#define TUSA_ANDROID_MAPSIMPLEPOINT_H

#include <vector>
#include <GLES2/gl2.h>

class MapSimplePoint {
public:
    std::vector<float> vertices;

    GLuint vbo;

    bool canBeDraw() {
        return vbo != 0;
    }
};


#endif //TUSA_ANDROID_MAPSIMPLEPOINT_H
