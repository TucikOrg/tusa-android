//
// Created by Artem on 30.09.2024.
//

#ifndef TUSA_ANDROID_MAPRENDERER_H
#define TUSA_ANDROID_MAPRENDERER_H

#include <string>
#include <vector>
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
    void renderFrame(bool isDebugBuildVariant);
    void init(AAssetManager *assetManager, JNIEnv *env, jobject& request_tile);
    void onSurfaceChanged(int w, int h);
    void onSurfaceCreated(AAssetManager* assetManager);
    void drag(float dx, float dy);
    float scale(float scaleFactor);
    void doubleTap();
    void destroyGL();
    int64_t confirmedClick(float x, float y, int64_t ignore);
    void deselectSelectedMarker();
    void cleanup();

    Markers& getMarkers() {  return markers; }
    MapControls& getMapControls() { return mapControls; }

    bool surfaceCreated = false;

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
    Markers markers = Markers(&mapFpsCounter);
    MapTileGetter* mapTileGetter;
    MapSymbols mapSymbols = MapSymbols();
    MapTest mapTest = MapTest();

    float forwardRenderingToWorldZoom = 6.0f;
    float planeSize = 10000000;
    int textureTileSizeUnit = 2048;
    std::string textureKey;



    void drawTestTexture(GLuint textureId, float width, float height, float shift = 0.1);

    void updateSumAllPlaceLabelsOfTiles(MapNumbers& mn, std::unordered_map<uint64_t, MapTile*> tiles, bool existTiles, int backgroundTilesSize);
};


#endif //TUSA_ANDROID_MAPRENDERER_H
