//
// Created by Artem on 16.09.2024.
//

#ifndef TUSA_ANDROID_MAPTILERENDER_H
#define TUSA_ANDROID_MAPTILERENDER_H

#include "csscolorparser/csscolorparser.h"
#include "shader/shaders_bucket.h"
#include "MapTile.h"
#include "MapCamera.h"
#include "MapStyle.h"
#include <GLES2/gl2.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/StdVector>

class MapTileRender {
public:
    void initTilesTexture();

    void renderTilesToTexture(
            ShadersBucket &shadersBucket,
            MapCamera &mapCamera,
            std::vector<MapTile *> tiles,
            Eigen::Matrix4f &pv
    );

    void renderTiles(
            ShadersBucket& shadersBucket,
            std::vector<MapTile*> tiles,
            Eigen::Matrix4f& pv
    );

    MapStyle& getStyle() {
        return style;
    }

    GLuint getTilesTexture() {
        return tilesTexture;
    }

private:
    MapStyle style = MapStyle();
    GLuint tilesTexture;
    GLuint tilesFrameBuffer;
    int tilesTextureWidth = 2024;
    int tilesTextureHeight = 2024;
};


#endif //TUSA_ANDROID_MAPTILERENDER_H
