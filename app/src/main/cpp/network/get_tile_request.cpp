//
// Created by Artem on 20.01.2024.
//

#include "network/get_tile_request.h"
#include <fstream>
#include <stdio.h>
#include <string>
#include <android/log.h>
#include "map/tile.h"
#include "style/style.h"
#include <filesystem>
#include <vector_tile.hpp>

#define LOG_TAG "GL_ARTEM"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

bool USE_CACHE_FOR_TILES = true;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

GetTileRequest::GetTileRequest(Cache *cache, JNIEnv *env): cache(cache), mainThreadEnv(env) {
    requestTileClassGlobal = reinterpret_cast<jclass>(env->NewGlobalRef(
            env->FindClass("com/artem/tusaandroid/RequestTile")));
    jmethodID constructor = env->GetMethodID(requestTileClassGlobal, "<init>", "()V");
    requestTileObjectGlobal = env->NewGlobalRef(env->NewObject(requestTileClassGlobal, constructor));
}

Tile* GetTileRequest::request(int x, int y, int zoom) {
    std::string key = Tile::makeKey(zoom, x, y);
    bool exist;
    std::string cachedData = cache->readCachedData(key.c_str(), exist);
    if(exist && USE_CACHE_FOR_TILES) {
        return new Tile(zoom, x, y, new vtzero::vector_tile(cachedData));
    }
    auto parallelThreadEnv = parallelThreadEnvironments[std::this_thread::get_id()];

    auto makeTileRequest = parallelThreadEnv->GetMethodID(requestTileClassGlobal, "request", "(III)[B");
    jbyteArray byteArray = (jbyteArray) parallelThreadEnv->CallObjectMethod(requestTileObjectGlobal, makeTileRequest, zoom, x, y);
    jsize length = parallelThreadEnv->GetArrayLength(byteArray);
    jbyte* bytes = parallelThreadEnv->GetByteArrayElements(byteArray, nullptr);
    std::string tileData((char*) bytes, length);

    cache->cacheData(key.c_str(), tileData);
    Tile* newTile = new Tile(
            zoom, x, y, new vtzero::vector_tile(tileData)
    );
    parallelThreadEnv->ReleaseByteArrayElements(byteArray, bytes, 0);
    return newTile;
}
