//
// Created by Artem on 07.02.2024.
//

#ifndef TUSA_TILES_STORAGE_H
#define TUSA_TILES_STORAGE_H

#include "network/get_tile_request.h"
#include "tile.h"
#include "style/style.h"

class TilesStorage {
public:
    TilesStorage(Cache* cache);
    Tile* getTile(int zoom, int x, int y);
    Tile* getOrLoad(int zoom, int x, int y, GetTileRequest* getTileRequest);
    bool existInMemory(int zoom, int x, int y);
private:
    std::map<std::string, Tile*> cacheTiles = {};
    Cache* cache;
};


#endif //TUSA_TILES_STORAGE_H
