//
// Created by Artem on 03.04.2024.
//

#ifndef TUSA_ANDROID_RENDER_TILE_COORDINATES_H
#define TUSA_ANDROID_RENDER_TILE_COORDINATES_H


#include "util/matrices.h"
#include "shader/shaders_bucket.h"
#include "symbols/symbols.h"
#include "tile_cords.h"

class RenderTileCoordinates {
public:
    RenderTileCoordinates(std::shared_ptr<ShadersBucket> shadersBucket, std::shared_ptr<Symbols> symbols);

    void render(Matrix4 pvmm, TileCords visibleTile, int extent, float frameWidth);

private:
    std::shared_ptr<ShadersBucket> shadersBucket;
    std::shared_ptr<Symbols> symbols;
};


#endif //TUSA_ANDROID_RENDER_TILE_COORDINATES_H
