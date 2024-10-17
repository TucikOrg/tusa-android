//
// Created by Artem on 16.09.2024.
//

#ifndef TUSA_ANDROID_MAPTILEGETTER_H
#define TUSA_ANDROID_MAPTILEGETTER_H

#include <map>
#include <stack>
#include <array>
#include <jni.h>
#include <thread>
#include <cmath>
#include "MapStyle.h"
#include "MapTile.h"
#include "CircularBuffer.h"

class MapTileGetter {
public:
    MapTileGetter(JNIEnv *env, jobject& request_tile);
    ~MapTileGetter();
    MapTile* getOrRequest(int x, int y, int z, bool forceMem = false);
    MapTile* findExistParent(int x, int y, int z);
    std::vector<MapTile*> findChildInPreviousTiles(int x, int y, int z);
    void clearActualTiles();
private:
    unsigned short networkThreadsCount = 2;
    std::unordered_map<uint64_t, MapTile*> cacheTiles = {};
    std::unordered_map<uint64_t, MapTile*> actualTiles = {};
    CircularBuffer<uint64_t, MapTile*> previousTilesBuffer = CircularBuffer<uint64_t, MapTile*>(18);

    jclass requestTileClassGlobal;
    jobject requestTileGlobal;
    JNIEnv *mainEnv;
    MapTile emptyTile = MapTile(-1, -1, -1);

    std::unordered_map<uint64_t, void*> pushedToNetwork;
    std::stack<std::array<int, 3>> networkTilesStack;

    MapTile* load(int x, int y, int z, JNIEnv* parallelThreadEnv);
    void networkTilesFunction(JavaVM* gJvm, short num);
};


#endif //TUSA_ANDROID_MAPTILEGETTER_H
