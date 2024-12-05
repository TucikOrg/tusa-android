//
// Created by Artem on 05.12.2024.
//

#ifndef TUSA_ANDROID_TILECACHEM_H
#define TUSA_ANDROID_TILECACHEM_H

#include <Eigen/Dense>
#include <Eigen/Core>
#include "MapTile.h"

struct TileCacheM {
    Eigen::Matrix4d& scaleMatrix ;
    Eigen::Matrix4d& vTileMatrix ;
    Eigen::Matrix4d& pvTileMatrix;
    MapTile* tile;
};


#endif //TUSA_ANDROID_TILECACHEM_H
