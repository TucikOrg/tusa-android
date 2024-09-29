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
            Eigen::Matrix4f pv
    );

    MapStyle& getStyle();
    GLuint getTilesTexture();
private:
    GLuint tilesTexture;
    GLuint tilesFrameBuffer;

    MapStyle style = MapStyle();
};


#endif //TUSA_ANDROID_MAPTILERENDER_H
