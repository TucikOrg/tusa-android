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
#include "MapTileGetter.h"
#include "renderer/render_tile_hash.h"
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
            MapCamera& mapCamera
    );

    void renderMainTexture(
            ShadersBucket& shadersBucket,
            MapCamera& mapCamera,
            MapTileGetter* mapTileGetter,
            std::vector<int> xTiles,
            std::vector<int> yTiles,
            int tilesZoom,
            float shiftX
    );

    GLuint getTileTexture(MapTile* tile, ShadersBucket& shadersBucket, MapCamera& mapCamera);

    GLuint renderTileToTexture(
            ShadersBucket& shadersBucket,
            MapTile* tile,
            MapCamera& mapCamera
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
    GLuint tileFrameBuffer;
    int textureSizeForTile = 2024;
    std::map<std::string, GLuint> textures;
};


#endif //TUSA_ANDROID_MAPTILERENDER_H
