//
// Created by Artem on 30.09.2024.
//

#ifndef TUSA_ANDROID_MAPRENDERER_H
#define TUSA_ANDROID_MAPRENDERER_H


#include "MapSymbols.h"
#include "MapTileGetter.h"
#include "MapFpsCounter.h"
#include "MapTileRender.h"
#include "MapControls.h"
#include "TileAndMatrix.h"
#include "MapEnvironment.h"
#include "DrawMapData.h"
#include "MapNumbers.h"
#include "DrawMap.h"
#include "Markers.h"
#include "AnimateCameraTo.h"
#include "TransferTilesToGPU.h"

class MapRenderer {
public:
    MapRenderer();
    void renderFrame();
    void init(AAssetManager *assetManager, JNIEnv *env, jobject& request_tile);
    void onSurfaceChanged(int w, int h);
    void onSurfaceCreated(AAssetManager* assetManager);
    void drag(float dx, float dy);
    void scale(float scaleFactor);
    void doubleTap();

    Markers& getMarkers() {  return markers; }
private:
    MapControls mapControls = MapControls();
    MapCamera mapCamera = MapCamera();
    MapTileRender mapTileRender = MapTileRender();
    ShadersBucket shadersBucket = ShadersBucket();
    MapFpsCounter mapFpsCounter = MapFpsCounter();
    MapEnvironment mapEnvironment = MapEnvironment();
    AnimateCameraTo animateCameraTo = AnimateCameraTo();
    TransferTilesToGPU transferTilesToGpu = TransferTilesToGPU();
    DrawMap drawMap = DrawMap();
    Markers markers = Markers();
    MapTileGetter* mapTileGetter;
    MapSymbols mapSymbols = MapSymbols();
    MapTest mapTest = MapTest();

    float forwardRenderingToWorldZoom = 13.0f;
    float planeSize = 10000000;
    int textureTileSizeUnit = 2048;
    std::string textureKey;

    GLuint testTextTexture = 0;
};


#endif //TUSA_ANDROID_MAPRENDERER_H