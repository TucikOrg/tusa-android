//
// Created by Artem on 15.10.2024.
//

#ifndef TUSA_ANDROID_DRAWMAP_H
#define TUSA_ANDROID_DRAWMAP_H

#include "DrawMapData.h"
#include <GLES2/gl2.h>
#include <Eigen/Dense>

class DrawMap {
public:
    void drawMap(DrawMapData& data);
private:
    float extent = 4096;
};


#endif //TUSA_ANDROID_DRAWMAP_H
