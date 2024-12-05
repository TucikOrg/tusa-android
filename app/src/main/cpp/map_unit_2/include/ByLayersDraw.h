//
// Created by Artem on 26.11.2024.
//

#ifndef TUSA_ANDROID_BYLAYERSDRAW_H
#define TUSA_ANDROID_BYLAYERSDRAW_H

#include "MapTile.h"
#include <Eigen/Dense>
#include <Eigen/Core>
#include <vector>

class ByLayersDraw {
public:
    MapTile* mapTile;
    Eigen::Matrix4f scaleMatrix ;
    Eigen::Matrix4f vTileMatrix ;
    Eigen::Matrix4f pvTileMatrix;
};


#endif //TUSA_ANDROID_BYLAYERSDRAW_H
