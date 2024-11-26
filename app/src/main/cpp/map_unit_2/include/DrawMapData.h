//
// Created by Artem on 15.10.2024.
//

#ifndef TUSA_ANDROID_DRAWMAPDATA_H
#define TUSA_ANDROID_DRAWMAPDATA_H

#include "MapCamera.h"
#include "MapTile.h"
#include "MapTileRender.h"
#include "MapEnvironment.h"
#include "util/eigen_gl.h"
#include <Eigen/Dense>

class DrawMapData {
public:
    bool& forwardRenderingToWorld;
    double& tilesSwiped;
    double& EPSGLonNormInfNegative;
    Eigen::Matrix4d& planeModelMatrix;
    float& leftXVertex;
    float& topYVertex;
    float& rightXVertex;
    float& bottomYVertex;
    int& yTilesAmount;
    Eigen::Matrix4d& pv;
    double& visXTilesDelta;
    MapCamera& mapCamera;
    std::unordered_map<uint64_t, MapTile*>& backgroundTiles;
    std::unordered_map<uint64_t, MapTile*>& tiles;
    short& tileZ;
    int& n;
    float& leftX;
    float& topY;
    MapTileRender& mapTileRender;
    Eigen::Matrix4d& view;
    Eigen::Matrix4d& projection;
    ShadersBucket& shadersBucket;
    float& zoom;
    int& visTileYStart;
    int& visTileYEnd;
    int& visTileXStartInf;
    int& visTileXEndInf;
    int& segments;
    double& planetVStart;
    double& planetVEnd;
    double& planetUStart;
    double& planetUEnd;
    double& planetVDelta;
    float& planeSize;
    double& verticesShift;
    double& planetUDelta;
    Eigen::Matrix4f& sphereModelMatrixFloat;
    float& transition;
    double& EPSG3857CamLat;
    float& shiftUTex;
    float& scaleUTex;
    MapEnvironment& mapEnvironment;
    MapSymbols& mapSymbols;
    MapFpsCounter& mapFpsCounter;
};


#endif //TUSA_ANDROID_DRAWMAPDATA_H
