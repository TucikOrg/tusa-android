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
    auto existsInMem = it != cacheTiles.end();
    if (existsInMem) {
        return it->second;
    }
    cacheMutex.unlock();

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
    if (empty) {
        cacheMutex.unlock();
        return nullptr;
    }
    cacheMutex.unlock();

    return it->second;
}

TilesStorage::TilesStorage() = default;

bool TilesStorage::existInMemory(int zoom, int x, int y) {
    std::string key = Tile::makeKey(zoom, x, y);
    auto it = cacheTiles.find(key);
    return it != cacheTiles.end();
}
