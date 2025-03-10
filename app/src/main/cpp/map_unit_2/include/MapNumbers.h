//
// Created by Artem on 15.10.2024.
//

#ifndef TUSA_ANDROID_MAPNUMBERS_H
#define TUSA_ANDROID_MAPNUMBERS_H

#include <cmath>
#include <Eigen/Dense>
#include <Eigen/Core>
#include "util/eigen_gl.h"
#include "MapCamera.h"
#include "shader/shaders_bucket.h"
#include "MapControls.h"

class MapNumbers {
public:
    MapNumbers() = default;
    MapNumbers(
            MapControls &mapControls,
            MapCamera &mapCamera,
            float planeSize,
            int textureMapSize,
            float forwardRenderingToWorldZoom
    );

    MapControls *mapControls;
    float planeSize;
    int textureTileSize;
    int n;
    short tileZ;
    double camLatitude;
    double camLongitude;
    double camLongitudeNormalized;
    double EPSG3857LonNormInf;
    double EPSG3857CamLatNorm;
    double EPSG3857CamLat;
    double EPSG4326CamLat;
    double camXStart;
    double distortion;
    float radius;
    double camXNorm;
    float distanceToMap;
    float maxDistanceToMap;
    float mapNearPlaneDelta;
    float zoomingDelta;
    float scale;
    float distortionDistanceToMapPortion;
    Eigen::Matrix4d planeModelMatrix;
    Eigen::Matrix4d sphereModelMatrix;
    Eigen::Matrix4d projection;
    Eigen::Matrix4d view;
    Eigen::Matrix4d pv;
    Eigen::Matrix4f pvFloat;
    Eigen::Matrix4f planeModelMatrixFloat;
    Eigen::Matrix4f sphereModelMatrixFloat;
    double visYTilesDelta;
    double visXTilesDelta;
    bool forwardRenderingToWorld;
    double camYStart;
    double camYEnd;
    int visTileYStart;
    int visTileYEnd;
    int visTileXStartInf;
    int visTileXEndInf;
    int xTilesAmount;
    int yTilesAmount;
    float scaleUTex;
    double planetVStart;
    double planetVEnd;
    float zoom;
    double tilesSwiped;
    double EPSGLonNormInfNegative;
    float topYVertex;
    float leftXVertex;
    float bottomYVertex;
    float rightXVertex;
    int segments;
    double verticesShift;
    double planetUStart;
    double planetUEnd;
    double planetVDelta;
    double planetUDelta;
    float transition;
    float shiftUTex;
    float screenWidth;
    float screenHeight;
    Eigen::Matrix4f pvScreen;
    float leftX;
    float topY;
};


#endif //TUSA_ANDROID_MAPNUMBERS_H
