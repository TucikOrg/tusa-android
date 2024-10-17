//
// Created by Artem on 04.10.2024.
//

#ifndef TUSA_ANDROID_TILEANDMATRIX_H
#define TUSA_ANDROID_TILEANDMATRIX_H

#include "MapTile.h"
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/StdVector>

class TileAndMatrix {
public:
    MapTile* tile;
    Eigen::Matrix4f matrix;
};


#endif //TUSA_ANDROID_TILEANDMATRIX_H
