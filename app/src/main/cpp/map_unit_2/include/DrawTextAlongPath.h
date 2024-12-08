//
// Created by Artem on 17.11.2024.
//

#ifndef TUSA_ANDROID_DRAWTEXTALONGPATH_H
#define TUSA_ANDROID_DRAWTEXTALONGPATH_H

#include <string>
#include <vector_tile.hpp>
#include <GLES2/gl2.h>
#include "Symbol.h"

class DrawTextAlongPath {
public:
    std::wstring wname;
    std::vector<float> path;
    std::vector<float> color;
    std::vector<std::tuple<Symbol, float, float, float>> forRender;
    float textWidth = 0;
    float textHeight = 0;
    float maxTop = 0;
    float legthOfPath;
    uint64_t featureId;
    uint64_t tileId;
    std::vector<vtzero::point> points;
    short type;

    bool transferedToGPU = false;
    GLuint vboData;
    GLuint ibo;
    unsigned int iboSize;

    float startAnimationTime = 0;
};


#endif //TUSA_ANDROID_DRAWTEXTALONGPATH_H
