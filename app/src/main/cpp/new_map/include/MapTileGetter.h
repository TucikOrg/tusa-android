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
#include "MapStyle.h"
#include "MapTile.h"

class MapTileGetter {
public:
    MapTileGetter(JNIEnv *env, jobject& request_tile, MapStyle& style);
    ~MapTileGetter();

    MapTile& getTile(int x, int y, int z);

    std::stack<std::array<int, 3>> networkTilesStack = {};
private:
    std::map<std::string, MapTile> cacheTiles = {};
    MapStyle& style;

    jclass requestTileClassGlobal;
    jobject requestTileGlobal;
    JNIEnv *mainEnv;
    MapTile emptyTile = MapTile();

    MapTile& getOrLoad(int x, int y, int z, JNIEnv* parallelThreadEnv);
    void networkTilesFunction(JavaVM* gJvm);
};


#endif //TUSA_ANDROID_MAPTILEGETTER_H
