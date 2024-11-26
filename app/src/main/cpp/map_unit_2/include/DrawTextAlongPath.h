//
// Created by Artem on 17.11.2024.
//

#ifndef TUSA_ANDROID_DRAWTEXTALONGPATH_H
#define TUSA_ANDROID_DRAWTEXTALONGPATH_H

#include <string>
#include <vector_tile.hpp>
#include "Symbol.h"

class DrawTextAlongPath {
public:
    std::wstring wname;
    std::vector<vtzero::point> points;
    std::vector<float> color;
    unsigned short type;
    std::vector<std::tuple<Symbol, float, float, float>> forRender;
    float textWidth = 0;
    float textHeight = 0;
    float maxTop = 0;
    float legthOfPath;
    float latitude;
    float longitude;
};


#endif //TUSA_ANDROID_DRAWTEXTALONGPATH_H
