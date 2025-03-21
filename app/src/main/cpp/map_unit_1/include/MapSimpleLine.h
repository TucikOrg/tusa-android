//
// Created by Artem on 16.09.2024.
//

#ifndef TUSA_ANDROID_MAPSIMPLELINE_H
#define TUSA_ANDROID_MAPSIMPLELINE_H

#include <vector>
#include <GLES2/gl2.h>

class MapSimpleLine {
public:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    GLuint vbo;
    GLuint ibo;
    size_t iboSize;

    bool canBeDraw() {
        return vbo != 0 && ibo != 0;
    }
};


#endif //TUSA_ANDROID_MAPSIMPLELINE_H
