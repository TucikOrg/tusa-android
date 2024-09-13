//
// Created by Artem on 07.02.2024.
//

#include "map/tiles_storage.h"
#include "util/android_log.h"
#include <fstream>

std::mutex cacheMutex;

Tile* TilesStorage::getOrLoad(int zoom, int x, int y, GetTileRequest* getTileRequest) {
    cacheMutex.lock();
    std::string key = Tile::makeKey(zoom, x, y);
    auto it = cacheTiles.find(key);
    auto exists = it != cacheTiles.end();
    cacheMutex.unlock();
    if (exists) {
        return it->second;
    }

    // network long time operation
    Tile* newTile = getTileRequest->request(x, y, zoom);

    cacheMutex.lock();
    cacheTiles.insert({key, newTile});
    cacheMutex.unlock();

    return newTile;
}

Tile* TilesStorage::getTile(int zoom, int x, int y) {
    cacheMutex.lock();
    std::string key = Tile::makeKey(zoom, x, y);
    auto it = cacheTiles.find(key);
    auto empty = it == cacheTiles.end();
    cacheMutex.unlock();
    if (empty) {
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
