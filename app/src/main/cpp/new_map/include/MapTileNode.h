//
// Created by Artem on 22.09.2024.
//

#ifndef TUSA_ANDROID_MAPTILENODE_H
#define TUSA_ANDROID_MAPTILENODE_H

#include "MapTile.h"
#include "MapTileGetter.h"

class MapTileNode {
public:
    MapTileNode(int x, int y, int z): x(x), y(y), z(z) {
        key = MapTile::makeKey(x, y, z);
    }

    std::vector<MapTileNode*> search(int x, int y, int z, MapTileGetter& storage, bool& isBackground) {
        auto isTheSameOne = this->x == x && this->y == y && this->z == z;
        if (isTheSameOne) {
            if (tile == nullptr || tile->isEmpty()) {
                // Cмотрим в хранилище если данных вектора нету в дереве
                tile = storage.getOrRequest(x, y, z);
                if (!tile->isEmpty()) {
                    return { this };
                }
            }
        }
    }

private:
    MapTileNode* parent = nullptr;
    MapTileNode* child00 = nullptr;
    MapTileNode* child01 = nullptr;
    MapTileNode* child10 = nullptr;
    MapTileNode* child11 = nullptr;

    MapTile* tile;

    int x;
    int y;
    int z;
    uint64_t key;
};


#endif //TUSA_ANDROID_MAPTILENODE_H
