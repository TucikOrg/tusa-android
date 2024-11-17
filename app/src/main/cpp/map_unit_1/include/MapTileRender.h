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
#include "TileAndMatrix.h"
#include "util/eigen_gl.h"
#include "MapTest.h"
#include "RenderTextureData.h"
#include "SmoothMapTextureRender.h"
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
            bool isForwardRendering,
            MapSymbols& mapSymbols,
            unsigned short from = 0,
            unsigned short to = 0
    );

    void renderTexture(RenderTextureData &data);

    GLuint getTilesTexture();
    GLuint getMapTexture() {
        return mapTexture;
    }
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

    float extent = 4096;
    GLuint tilesTexture;
    GLuint tilesFrameBuffer;
    int prTex2dHeight;
    int prTex2dWidth;
    GLuint mapTexture;
    GLuint mapTextureFramebuffer;
};


#endif //TUSA_ANDROID_MAPTILERENDER_H
