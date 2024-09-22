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
#include "MapSymbols.h"
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/StdVector>

class MapTest {
public:
    void drawTilesTextureTest(
            ShadersBucket& shadersBucket,
            MapCamera& mapCamera,
            GLuint renderMapTexture,
            int xSize,
            int ySize
    );

    void drawFPS(ShadersBucket &shadersBucket, MapSymbols &mapSymbols, MapCamera& mapCamera, float fps);
    void drawPoints2D(ShadersBucket& shadersBucket, std::vector<float>& vertices, float pointSize, Eigen::Matrix4f& matrix);
    void drawPoints3D(ShadersBucket& shadersBucket, std::vector<float>& vertices, float pointSize, Eigen::Matrix4f& matrix);
    void drawLines3D(ShadersBucket& shadersBucket, std::vector<float>& vertices, std::vector<unsigned int> &indices, float lineWidth, Eigen::Matrix4f& matrix);

    void init(MapCamera& mapCamera);
private:
    Eigen::Matrix4f pvUI;
};


#endif //TUSA_ANDROID_MAPTEST_H
