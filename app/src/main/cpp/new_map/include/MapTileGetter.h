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
    MapTile* getOrRequest(int x, int y, int z);
private:
    std::map<std::string, MapTile> cacheTiles = {};
    MapStyle& style;

    jclass requestTileClassGlobal;
    jobject requestTileGlobal;
    JNIEnv *mainEnv;
    MapTile emptyTile = MapTile(-1, -1, -1);

    std::map<std::string, void*> pushedToNetwork;
    std::stack<std::array<int, 3>> networkTilesStack = {};

    MapTile* load(int x, int y, int z, JNIEnv* parallelThreadEnv);
    void networkTilesFunction(JavaVM* gJvm);
};


#endif //TUSA_ANDROID_MAPTILEGETTER_H
