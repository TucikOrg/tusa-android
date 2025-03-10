//
// Created by Artem on 15.10.2024.
//

#ifndef TUSA_ANDROID_RENDERTEXTUREDATA_H
#define TUSA_ANDROID_RENDERTEXTUREDATA_H

#include "MapCamera.h"
#include "MapTile.h"
#include "shader/shaders_bucket.h"

struct RenderTextureData {
public:
    int textureTileSize;
    int xTilesAmount;
    int yTilesAmount;
    std::unordered_map<uint64_t, MapTile*> backgroundTiles;
    std::unordered_map<uint64_t, MapTile*> tiles ;
    float zoom;
    bool forwardRenderingToWorld;
    short tileZ;
    int n;
    float leftX;
    float topY;
    int visTileYStart;
    int visTileYEnd;
    int visTileXStartInf;
    int visTileXEndInf;
};


#endif //TUSA_ANDROID_RENDERTEXTUREDATA_H
