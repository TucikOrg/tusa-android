//
// Created by Artem on 07.02.2024.
//

#include "map/tiles_storage.h"
#include "util/android_log.h"
#include <fstream>

Tile* TilesStorage::getOrLoad(int zoom, int x, int y, GetTileRequest* getTileRequest) {
    std::string key = Tile::makeKey(zoom, x, y);
    auto it = cacheTiles.find(key);
    auto exists = it != cacheTiles.end();
    if (exists) {
        return it->second;
    }

    Tile* newTile = getTileRequest->request(x, y, zoom);
    cacheTiles.insert({key, newTile});
    return newTile;
}

Tile* TilesStorage::getTile(int zoom, int x, int y) {
    std::string key = Tile::makeKey(zoom, x, y);
    auto it = cacheTiles.find(key);
    if (it == cacheTiles.end()) {
        return nullptr;
    }
    return it->second;
}

TilesStorage::TilesStorage(Cache* cache)
    : cache(cache) { }

bool TilesStorage::existInMemory(int zoom, int x, int y) {
    std::string key = Tile::makeKey(zoom, x, y);
    auto it = cacheTiles.find(key);
    return it != cacheTiles.end();
}
