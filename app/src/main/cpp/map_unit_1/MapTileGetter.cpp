//
// Created by Artem on 16.09.2024.
//

#include "MapTileGetter.h"

#include "util/android_log.h"

std::mutex updateVisibleTiles;
std::mutex cacheMutex2;
std::mutex networkTileStackMutex2;

MapTileGetter::~MapTileGetter() {
    mainEnv->DeleteGlobalRef(requestTileClassGlobal);
    mainEnv->DeleteGlobalRef(requestTileGlobal);
}

MapTileGetter::MapTileGetter(JNIEnv *env, jobject& request_tile, MapSymbols& mapSymbols): mainEnv(env), mapSymbols(mapSymbols) {
    JavaVM* gJvm = nullptr;
    env->GetJavaVM(&gJvm);

    requestTileClassGlobal = static_cast<jclass>(env->NewGlobalRef(env->FindClass("com/artem/tusaandroid/RequestTile")));
    requestTileGlobal = env->NewGlobalRef(request_tile);

    for (short i = 0; i <= networkThreadsCount; i++) {
        std::thread networkTileThread([this, gJvm, i] { this->networkTilesFunction(gJvm, i); });
        networkTileThread.detach();
        networkThreads.push_back(std::move(networkTileThread));
    }
}

void MapTileGetter::joinThreads() {
    for (auto& thread : networkThreads) {
        if (thread.joinable())
            thread.join();
    }
    threadsActive = false;
}

void MapTileGetter::clearActualTiles() {
    updateVisibleTiles.lock();
    actualTiles.clear();
    updateVisibleTiles.unlock();
}

void MapTileGetter::networkTilesFunction(JavaVM* gJvm, short num) {
    JNIEnv* threadEnv;
    gJvm->AttachCurrentThread(&threadEnv, NULL);

    while (threadsActive) {
        // Берем тайлы для загрузки из списка и грузим потом
        bool exists = false;
        networkTileStackMutex2.lock();
        std::array<int, 3> tileToNetwork;
        auto isEmpty = networkTilesStack.empty();
        if (!isEmpty) {
            tileToNetwork = networkTilesStack.top();
            networkTilesStack.pop();
            exists = true;
        }
        networkTileStackMutex2.unlock();
        if (!exists) continue;

        // Проверяем что тайл актуален и есть смысл его загружать к данному моменту
        // в момент когда наступает его очередь загрузки может быть так что он уже не нужен
        auto tileToNetworkKey = MapTile::makeKey(tileToNetwork[0], tileToNetwork[1], tileToNetwork[2]);
        updateVisibleTiles.lock();
        bool isActual = actualTiles.find(tileToNetworkKey) == actualTiles.end();
        updateVisibleTiles.unlock();

        if (isActual) {
            // Загружаем тайл
            //LOGI("Load tile(%d) %d %d %d", num, tileToNetwork[0], tileToNetwork[1], tileToNetwork[2]);
            auto loaded = load(tileToNetwork[0], tileToNetwork[1], tileToNetwork[2], threadEnv);
            //LOGI("Tile ready (%d)", num);
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
    MapTile* mapTile = new MapTile(x, y, z, vectorTile, mapSymbols);

    cacheMutex2.lock();
    cacheTiles.insert({key, mapTile});
    cacheMutex2.unlock();

    return mapTile;
}

MapTile* MapTileGetter::getOrRequest(int x, int y, int z, bool forceMem) {
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
        previousTilesBuffer.add(key, it->second);
        actualTiles.insert({key, it->second});
        cacheMutex2.unlock();
        return it->second;
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
    MapTile* parent = root->second;
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
            previousTilesBuffer.add(key, it->second);
            parent = it->second;
            break;
        }

        if (parentX <= 0 && parentY <= 0 && parentZ <= 0)
            break;
    }

    cacheMutex2.unlock();
    return parent;
}

std::vector<MapTile*> MapTileGetter::findChildInPreviousTiles(int x, int y, int z) {
    cacheMutex2.lock();
    std::vector<MapTile*> childrens = {};
    auto previousTiles = previousTilesBuffer.getBuffer();
    int lastIndex = previousTiles.size() - 1;
    for (int i = lastIndex - 1; i >= 0; i--) {
        auto tile = previousTiles[i].second;
        if (tile == nullptr) continue;

        auto cover = MapTile::coverOneOther(x, y, z, tile->getX(), tile->getY(), tile->getZ());
        if (!cover) continue;

        bool coveredAlready = false;
        for (auto child : childrens) {
            auto cover2 = MapTile::coverOneOther(child->getX(), child->getY(), child->getZ(), tile->getX(), tile->getY(), tile->getZ());
            if (cover2) {
                coveredAlready = true;
                break;
            }
        }
        if (coveredAlready) continue;

        childrens.push_back(tile);
    }

    cacheMutex2.unlock();
    return childrens;
}

void MapTileGetter::destroy() {
    for (auto& tilePair : cacheTiles) {
        auto tile = tilePair.second;
        tile->destroy();
        delete tile;
    }
    cacheTiles.clear();
}
