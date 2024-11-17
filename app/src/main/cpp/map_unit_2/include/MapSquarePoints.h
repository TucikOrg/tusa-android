//
// Created by Artem on 10.10.2024.
//

#ifndef TUSA_ANDROID_MAPSQUAREPOINTS_H
#define TUSA_ANDROID_MAPSQUAREPOINTS_H

#include <vector>
#include <GLES2/gl2.h>

class MapSquarePoints {
public:
    std::vector<float> vertices;
    std::vector<float> uvs;
    std::vector<unsigned int> indices;
    std::vector<float> shifts;

    GLuint vbo;
    GLuint vboUVs;
    GLuint ibo;
    GLuint vboShifts;
    size_t iboSize;

    bool canBeDraw() {
        return vbo != 0;
    }
};


#endif //TUSA_ANDROID_MAPSQUAREPOINTS_H
