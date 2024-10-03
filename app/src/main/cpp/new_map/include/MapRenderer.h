//
// Created by Artem on 16.09.2024.
//

#ifndef TUSA_ANDROID_MAPRENDERER2_H
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
#include "MapSymbols.h"
#include <vector>


class MapRenderer {

public:
    MapRenderer();
    ~MapRenderer();

    void renderFrame();
    void init(AAssetManager *assetManager, JNIEnv *env, jobject& request_tile);
    void onSurfaceChanged(int w, int h);
    void onSurfaceCreated(AAssetManager* assetManager);
    void drag(float dx, float dy);
    void scale(float scaleFactor);
    void doubleTap();
    void render2DMap();
private:
    ShadersBucket shadersBucket = ShadersBucket();
    MapGeometry mapGeometry = MapGeometry();
    MapControls mapControls = MapControls();
    MapCamera mapCamera;
    MapTest mapTest;
    MapFpsCounter mapFpsCounter = MapFpsCounter();
    MapTileRender mapTileRender;
    MapTileGetter* mapTileGetter;
    MapSymbols mapSymbols = MapSymbols();


    float distortion(float latitudeRadians) {
        return 1.0f / cosf(latitudeRadians);
    }

    float planeWidth = 3000000.0f;
    float radius = planeWidth / (distortion(DEG2RAD(85.0511)) / 2.0);

    float prTex2dWidth = 0;
    float prTex2dHeight = 0;
    float prTexPlanetWidth = 0;
    float prTexPlanetHeight = 0;


    std::string textureKey = "";
    GLuint mapTexture2D;
    GLuint mapTextureForPlanet;
    GLuint mapFrameBuffer2D;
    GLuint mapFrameBufferForPlanet;
};


#endif //TUSA_ANDROID_MAPRENDERER2_H
