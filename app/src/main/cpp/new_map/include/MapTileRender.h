//
// Created by Artem on 16.09.2024.
//

#ifndef TUSA_ANDROID_MAPTILERENDER_H
#define TUSA_ANDROID_MAPTILERENDER_H

#include "csscolorparser/csscolorparser.h"
#include "shader/shaders_bucket.h"
#include "MapTile.h"
#include "MapTest.h"
#include "MapCamera.h"
#include "MapStyle.h"
#include "MapTileGetter.h"
#include "MapControls.h"
#include "MapTilePos.h"
#include "TileAndMatrix.h"
#include "util/eigen_gl.h"
#include "MapTest.h"
#include <GLES2/gl2.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/StdVector>

class MapTileRender {
public:
    void initTilesTexture();

    void renderTile(
            ShadersBucket& shadersBucket,
            MapTile* tile,
            MapCamera& mapCamera,
            Eigen::Matrix4f p,
            Eigen::Matrix4f vm,
            Eigen::Matrix4f pvm,
            float zoom,
            bool isForwardRendering
    );

    GLuint getTilesTexture();


private:
    void drawBackground(
            ShadersBucket& shadersBucket,
            Eigen::Matrix4f pvm
    );

    void drawLayer(
            ShadersBucket& shadersBucket,
            MapTile* tile,
            Eigen::Matrix4f p,
            Eigen::Matrix4f vm,
            Eigen::Matrix4f pvm,
            int styleIndex,
            float zoom,
            bool isForwardRendering
    );

    GLuint tilesTexture;
    GLuint tilesFrameBuffer;
};


#endif //TUSA_ANDROID_MAPTILERENDER_H
