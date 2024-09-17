//
// Created by Artem on 20.01.2024.
//

#ifndef TUSA_REQUEST_H
#define TUSA_REQUEST_H

#include <jni.h>
#include <thread>
#include "map/tile.h"
#include "style/style.h"

class GetTileRequest {
public:
    GetTileRequest(JNIEnv *env, jobject requestTileObject);
    ~GetTileRequest() {
        mainThreadEnv->DeleteGlobalRef(requestTileObjectGlobal);
        mainThreadEnv->DeleteGlobalRef(requestTileClassGlobal);
    }

    Tile* request(int x, int y, int z);

    void setEnv(JNIEnv* jniEnv) {
        parallelThreadEnvironments[std::this_thread::get_id()] = jniEnv;
    }
private:
    JNIEnv *mainThreadEnv;
    std::map<std::thread::id, JNIEnv*> parallelThreadEnvironments;

    jclass requestTileClassGlobal;
    jobject requestTileObjectGlobal;
};


#endif //TUSA_REQUEST_H
