//
// Created by Artem on 16.09.2024.
//

#ifndef TUSA_ANDROID_MAPRENDERER_H
#define TUSA_ANDROID_MAPRENDERER_H


#include <android/asset_manager.h>
#include <jni.h>
#include "shader/shaders_bucket.h"
#include "MapCamera.h"
#include "MapFpsCounter.h"
#include "MapGeometry.h"
#include "MapTest.h"
#include "MapTileGetter.h"
#include "MapTileRender.h"
#include "MapControls.h"
#include <vector>


class MapRenderer {

public:
    ~MapRenderer();

    void renderFrame();
    void init(AAssetManager *assetManager, JNIEnv *env, jobject& request_tile);
    void onSurfaceChanged(int w, int h);
    void onSurfaceCreated(AAssetManager* assetManager);
    void drag(float dx, float dy);
    void scale(float scaleFactor);
private:
    void productionRender();

private:
    ShadersBucket shadersBucket = ShadersBucket();
    MapGeometry mapGeometry = MapGeometry();
    MapControls mapControls = MapControls();
    MapCamera mapCamera;
    MapTest mapTest;
    MapFpsCounter mapFpsCounter = MapFpsCounter();
    MapTileRender mapTileRender;
    MapTileGetter* mapTileGetter;
};


#endif //TUSA_ANDROID_MAPRENDERER_H
