//
// Created by Artem on 16.09.2024.
//

#ifndef TUSA_ANDROID_MAPTEST_H
#define TUSA_ANDROID_MAPTEST_H


#include "shader/shaders_bucket.h"
#include "MapCamera.h"
#include "MapGeometry.h"
#include "MapTile.h"
#include "MapTileGetter.h"
#include "MapTileRender.h"
#include "util/android_log.h"
#include "MapStyle.h"
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/StdVector>

class MapTest {
public:
    void drawPlainGeometryTest(
            ShadersBucket& shadersBucket,
            MapCamera& mapCamera,
            MapGeometry& mapGeometry
    );
    void drawTileGeometryTest(
            ShadersBucket& shadersBucket,
            MapCamera& mapCamera,
            MapGeometry& mapGeometry,
            MapTile& mapTile,
            MapTileRender& mapTileRender
    );
    void drawRootTileGeometryTest(
            ShadersBucket& shadersBucket,
            MapCamera& mapCamera,
            MapGeometry& mapGeometry,
            MapTileGetter &mapTileGetter,
            MapTileRender &mapTileRender
    );
    void drawTilesToTextureTest(
            ShadersBucket& shadersBucket,
            MapCamera& mapCamera,
            MapGeometry& mapGeometry,
            MapTileGetter &mapTileGetter,
            MapTileRender &mapTileRender
    );
    void drawPlainTilesTextureTest(
            ShadersBucket& shadersBucket,
            MapCamera& mapCamera,
            MapGeometry& mapGeometry,
            MapTileRender& mapTileRender
    );

    void drawPoints2D(ShadersBucket& shadersBucket, std::vector<float>& vertices, float pointSize, Eigen::Matrix4f& matrix);
};


#endif //TUSA_ANDROID_MAPTEST_H
