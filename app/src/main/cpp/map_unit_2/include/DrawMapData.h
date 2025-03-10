//
// Created by Artem on 15.10.2024.
//

#ifndef TUSA_ANDROID_DRAWMAPDATA_H
#define TUSA_ANDROID_DRAWMAPDATA_H

#include "MapCamera.h"
#include "MapTile.h"
#include "MapTileRender.h"
#include "MapEnvironment.h"
#include "util/eigen_gl.h"
#include <Eigen/Dense>

class DrawMapData {
public:
    std::unordered_map<uint64_t, MapTile*> backgroundTiles;
    std::unordered_map<uint64_t, MapTile*> tiles;
    MapNumbers mapNumbers;
};


#endif //TUSA_ANDROID_DRAWMAPDATA_H
