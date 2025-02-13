//
// Created by Artem on 15.10.2024.
//

#ifndef TUSA_ANDROID_DRAWMAP_H
#define TUSA_ANDROID_DRAWMAP_H

#include "DrawMapData.h"
#include "OnTilePathText.h"
#include "ReservedList.h"
#include "TileCacheM.h"
#include <GLES2/gl2.h>
#include <Eigen/Dense>

class DrawMap {
public:
    void onSurfaceCreated();
    void drawMapForward(DrawMapData& data);
    void drawMapViaTexture(DrawMapData& data);
    void destroy();
private:
    float extent = 4096;

    short savedTileZ = 0;
    short savedSegmentsAmount = 0;

    unsigned int indicesSize;
    GLuint EPSG3857_VBO;
    GLuint verticesVBO;
    GLuint indicesIBO;
    GLuint texUV_VBO;
};


#endif //TUSA_ANDROID_DRAWMAP_H
