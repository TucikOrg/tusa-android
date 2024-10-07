//
// Created by Artem on 30.09.2024.
//

#ifndef TUSA_ANDROID_MAPRENDERER2_H
#define TUSA_ANDROID_MAPRENDERER2_H


#include "MapSymbols.h"
#include "MapTileGetter.h"
#include "MapFpsCounter.h"
#include "MapTileRender.h"
#include "MapControls2.h"
#include "TileAndMatrix.h"

class MapRenderer2 {
public:
    MapRenderer2();
    void renderFrame();
    void init(AAssetManager *assetManager, JNIEnv *env, jobject& request_tile);
    void onSurfaceChanged(int w, int h);
    void onSurfaceCreated(AAssetManager* assetManager);
    void drag(float dx, float dy);
    void scale(float scaleFactor);
    void doubleTap();
private:
    MapControls2 mapControls = MapControls2();
    MapCamera mapCamera = MapCamera();
    MapTileRender mapTileRender = MapTileRender();
    ShadersBucket shadersBucket = ShadersBucket();
    MapFpsCounter mapFpsCounter = MapFpsCounter();
    MapTileGetter* mapTileGetter;
    MapSymbols mapSymbols = MapSymbols();
    MapTest mapTest = MapTest();

    float normalizeXTile(float infTile, int n) {
        return infTile < 0 ? fmod(fmod(infTile, n) + n, n) : fmod(infTile, n);
    }

    float planeSize = 10000000;
    int textureTileSizeUnit = 512 * 2.0;
    int prTex2dHeight;
    int prTex2dWidth;
    std::string textureKey;

    GLuint mapTexture;
    GLuint mapTextureFramebuffer;
};


#endif //TUSA_ANDROID_MAPRENDERER2_H
