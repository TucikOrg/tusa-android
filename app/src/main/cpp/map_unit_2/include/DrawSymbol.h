//
// Created by Artem on 27.11.2024.
//

#ifndef TUSA_ANDROID_DRAWSYMBOL_H
#define TUSA_ANDROID_DRAWSYMBOL_H

#include <vector>
#include <Eigen/Dense>

class DrawSymbol {
public:
    std::vector<float> vertices;
    std::vector<float> textureCords;
    Eigen::Matrix4f vmChar;
};


#endif //TUSA_ANDROID_DRAWSYMBOL_H
