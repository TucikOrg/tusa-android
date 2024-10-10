//
// Created by Artem on 16.09.2024.
//

#include "MapTileGetter.h"

#include "util/android_log.h"

std::mutex cacheMutex2;
std::mutex networkTileStackMutex2;

MapTileGetter::~MapTileGetter() {
    mainEnv->DeleteGlobalRef(requestTileClassGlobal);
    mainEnv->DeleteGlobalRef(requestTileGlobal);
}

MapTileGetter::MapTileGetter(JNIEnv *env, jobject& request_tile): mainEnv(env) {
    JavaVM* gJvm = nullptr;
    env->GetJavaVM(&gJvm);

    requestTileClassGlobal = static_cast<jclass>(env->NewGlobalRef(env->FindClass("com/artem/tusaandroid/RequestTile")));
    requestTileGlobal = env->NewGlobalRef(request_tile);

    std::thread networkTileThread([this, gJvm] { this->networkTilesFunction(gJvm); });
    networkTileThread.detach();
}

void MapTileGetter::networkTilesFunction(JavaVM* gJvm) {
    JNIEnv* threadEnv;
    gJvm->AttachCurrentThread(&threadEnv, NULL);

    while (true) {
        // Берем тайлы для загрузки из списка и грузим потом
        bool shouldLoadTile = false;
        networkTileStackMutex2.lock();
        std::array<int, 3> tileToNetwork;
        auto isEmpty = networkTilesStack.empty();
        if (!isEmpty) {
            tileToNetwork = networkTilesStack.top();
            networkTilesStack.pop();
            shouldLoadTile = true;
        }
        networkTileStackMutex2.unlock();

        // Проверяем что тайл актуален и есть смысл его загружать к данному моменту
        // в момент когда наступает его очередь загрузки может быть так что он уже не нужен
        //        visibleTilesResultMutex.lock();
        //        if (visibleTilesResult.newVisibleTiles.find(tileToNetwork.getKey()) == visibleTilesResult.newVisibleTiles.end()) {
        //            shouldLoadTile = false;
        //        }
        //        visibleTilesResultMutex.unlock();

        if (shouldLoadTile) {
            // Загружаем тайл
            auto loaded = load(tileToNetwork[0], tileToNetwork[1], tileToNetwork[2], threadEnv);
        }
    }

    gJvm->DetachCurrentThread();
}

MapTile* MapTileGetter::load(int x, int y, int z, JNIEnv *parallelThreadEnv) {
    auto key = MapTile::makeKey(x, y, z);

    auto makeTileRequest = parallelThreadEnv->GetMethodID(requestTileClassGlobal, "request", "(III)[B");
    jbyteArray byteArray = (jbyteArray) parallelThreadEnv->CallObjectMethod(requestTileGlobal, makeTileRequest, z, x, y);
    jsize length = parallelThreadEnv->GetArrayLength(byteArray);
    jbyte* bytes = parallelThreadEnv->GetByteArrayElements(byteArray, nullptr);
    std::string tileData((char*) bytes, length);
    parallelThreadEnv->ReleaseByteArrayElements(byteArray, bytes, 0);

    vtzero::vector_tile vectorTile(tileData);
    MapTile mapTile = MapTile(x, y, z, vectorTile);

    cacheMutex2.lock();
    cacheTiles.insert({key, std::move(mapTile)});
    cacheMutex2.unlock();

    return &cacheTiles.find(key)->second;
}

MapTile *MapTileGetter::getOrRequest(int x, int y, int z, bool forceMem) {
    cacheMutex2.lock();
    auto key = MapTile::makeKey(x, y, z);
    auto req = pushedToNetwork.find(key);
    auto reqExists = req != pushedToNetwork.end();
    auto it = cacheTiles.find(key);
    auto existsInMem = it != cacheTiles.end();
    if (existsInMem) {
        if (reqExists) {
            pushedToNetwork.erase(req);
        }
        cacheMutex2.unlock();
        return &it->second;
    }
    cacheMutex2.unlock();

    if (!reqExists && !forceMem) {
        pushedToNetwork.insert({key, nullptr });
        networkTileStackMutex2.lock();
        networkTilesStack.push({x, y, z});
        networkTileStackMutex2.unlock();
    }

    return &emptyTile;
}

MapTile *MapTileGetter::findExistParent(int x, int y, int z) {
    cacheMutex2.lock();
    auto rootKey = MapTile::makeKey(0, 0, 0);
    auto root = cacheTiles.find(rootKey);
    if (root == cacheTiles.end()) {
        cacheMutex2.unlock();
        return &emptyTile;
    }
    MapTile* parent = &root->second;
    int parentX = x;
    int parentY = y;
    int parentZ = z;
    while (parent->isRoot()) {
        parentX = floor(FLOAT(parentX) / 2.0f);
        parentY = floor(FLOAT(parentY) / 2.0f);
        parentZ = parentZ - 1;
        auto key = MapTile::makeKey(parentX, parentY, parentZ);
        auto it = cacheTiles.find(key);
        auto existsInMem = it != cacheTiles.end();
        if (existsInMem) {
            parent = &it->second;
            break;
        }

        if (parentX <= 0 && parentY <= 0 && parentZ <= 0)
            break;
    }

    cacheMutex2.unlock();
    return parent;
}


