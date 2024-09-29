//
// Created by Artem on 25.09.2024.
//

#ifndef TUSA_ANDROID_MAPTILEPOS_H
#define TUSA_ANDROID_MAPTILEPOS_H

#include <cmath>

class MapTilePos {
public:
    MapTilePos(int x, int y, int z, float planeWidth) : x(x), y(y), z(z) {
        tileSize  = planeWidth / pow(2, z);
    }

    int x;
    int y;
    int z;
    float tileSize;

    bool is000() {
        return x == 0 && y == 0 && z == 0;
    }
};


#endif //TUSA_ANDROID_MAPTILEPOS_H
