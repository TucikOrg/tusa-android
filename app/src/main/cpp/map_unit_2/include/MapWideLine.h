//
// Created by Artem on 07.10.2024.
//

#ifndef TUSA_ANDROID_MAPWIDELINE_H
#define TUSA_ANDROID_MAPWIDELINE_H

#include <vector>
#include <GLES2/gl2.h>

class MapWideLine {
public:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> perpendiculars;
    std::vector<float> uv;

    GLuint vboUv;
    GLuint vbo;
    GLuint vboPerpendiculars;
    GLuint ibo;
    size_t iboSize;
    size_t verticesSize;

    bool canBeDraw() {
        return vbo != 0 && ibo != 0;
    }
};


#endif //TUSA_ANDROID_MAPWIDELINE_H
