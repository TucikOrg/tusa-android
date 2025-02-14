 //
// Created by Artem on 30.09.2024.
//

#include "MapRenderer.h"
#include <cmath>
#include "MapColors.h"
#include <format>
#include <iostream>

void MapRenderer::renderFrame(bool isDebugBuildVariant) {
    mapFpsCounter.newFrame();
    auto mn = MapNumbers(
            mapControls, mapCamera, planeSize,
            textureTileSizeUnit, forwardRenderingToWorldZoom
    );

    animateCameraTo.animateTick(mapFpsCounter, mapControls);

    // определяем тайлы и ключ
    bool allTilesReady = true;
    int existTiles = 0;
    std::unordered_map<uint64_t, MapTile*> backgroundTiles = {};
    std::unordered_map<uint64_t, MapTile*> tiles = {};
    for (int tileY = mn.visTileYStart; tileY < mn.visTileYEnd; tileY++) {
        for (int tileXInf = mn.visTileXStartInf, xPos = 0; tileXInf < mn.visTileXEndInf; tileXInf++, xPos++) {
            int tileX = Utils::normalizeXTile(tileXInf, mn.n);
            auto tile = mapTileGetter->getOrRequest(tileX, tileY, mn.tileZ);
            if (tile->isEmpty()) {
                allTilesReady = false;
                std::vector<MapTile*> replacement(1);
                // Нужна замена на фоновый тайл
                if (mn.zoomingDelta >= 0.0f) {
                    replacement[0] = mapTileGetter->findExistParent(tileX, tileY, mn.tileZ);
                } else {
                    replacement = mapTileGetter->findChildInPreviousTiles(tileX, tileY, mn.tileZ);
                }

                for (auto replace : replacement) {
                    auto addKey = MapTile::makeKey(replace->getX(), replace->getY(), replace->getZ());
                    if (backgroundTiles.find(addKey) == backgroundTiles.end()) {
                        backgroundTiles.insert({addKey, replace});
                    }
                }
            }
            existTiles += !tile->isEmpty();
            tiles.insert({MapTile::makeKey(tileX, tileY, mn.tileZ), tile});
        }
    }


    // проверяем что тайлы храняться в GPU
    // если они не в GPU то грузим в GPU
    transferTilesToGpu.transfer(tiles);
    transferTilesToGpu.transfer(backgroundTiles);

    mapTileGetter->clearActualTiles();
    if (allTilesReady)
        mapControls.allTilesReady();

    std::string newTextureKey =
            std::to_string(mn.visTileYStart) +
            std::to_string(mn.visTileYEnd) +
            std::to_string(mn.visTileXStartInf) +
            std::to_string(mn.visTileXEndInf) +
            std::to_string(mn.tileZ) +
            std::to_string(backgroundTiles.size()) +
            std::to_string(textureTileSizeUnit) +
            std::to_string(existTiles);

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    float topY = mn.visTileYStart;
    float leftX = Utils::normalizeXTile(mn.visTileXStartInf, mn.n);

    //////////////////////////////////
    ////                            //
    ////       Render texture       //
    ////                            //
    //////////////////////////////////
    if (textureKey != newTextureKey && !mn.forwardRenderingToWorld) {
        textureKey = newTextureKey;
        RenderTextureData data = {
                mn.textureTileSize,
                mn.xTilesAmount,
                mn.yTilesAmount,
                mapCamera,
                backgroundTiles,
                tiles,
                mn.zoom,
                shadersBucket,
                mn.forwardRenderingToWorld,
                mn.tileZ,
                mn.n, leftX, topY, mn.visTileYStart, mn.visTileYEnd,
                mn.visTileXStartInf, mn.visTileXEndInf,
                mapSymbols, mn
        };

        mapTileRender.renderTexture(data);
    }

    //////////////////////////////////
    ////                            //
    ////       Render planet        //
    ////                            //
    //////////////////////////////////
    DrawMapData drawMapData = {
        mn.forwardRenderingToWorld, mn.tilesSwiped, mn.EPSGLonNormInfNegative, mn.planeModelMatrix,
        mn.leftXVertex, mn.topYVertex, mn.rightXVertex, mn.bottomYVertex, mn.yTilesAmount,
        mn.pv, mn.visXTilesDelta, mapCamera, backgroundTiles, tiles, mn.tileZ, mn.n,
        leftX, topY, mapTileRender, mn.view, mn.projection, shadersBucket, mn.zoom,
        mn.visTileYStart, mn.visTileYEnd, mn.visTileXStartInf, mn.visTileXEndInf, mn.segments,
        mn.planetVStart, mn.planetVEnd, mn.planetUStart, mn.planetUEnd, mn.planetVDelta, planeSize, mn.verticesShift,
        mn.planetUDelta, mn.sphereModelMatrixFloat, mn.transition, mn.EPSG3857CamLat,
        mn.shiftUTex, mn.scaleUTex, mapEnvironment, mapSymbols, mapFpsCounter, mn
    };

    mapEnvironment.selectClearColor(mn.zoom);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mapEnvironment.draw(mn, shadersBucket);
    if (mn.forwardRenderingToWorld) {
        glBindTexture(GL_TEXTURE_2D, mapSymbols.getAtlasTexture());
        drawMap.drawMapForward(drawMapData);
    } else {
        glBindTexture(GL_TEXTURE_2D, mapTileRender.getMapTexture());
        drawMap.drawMapViaTexture(drawMapData);
        glBindTexture(GL_TEXTURE_2D, mapSymbols.getAtlasTexture());
    }


    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    bool canRefreshTitles = backgroundTiles.size() == 0;
    markers.drawMarkers(shadersBucket, mn.pv,
                        mn, tiles, mapSymbols, mapCamera,
                        canRefreshTitles
    );

    if (isDebugBuildVariant) {
        auto fps = Utils::floatToWString(mapFpsCounter.getFps(), 1);
        auto zoomText = Utils::floatToWString(mn.zoom, 1);
        std::wstring textInfo = L"FPS: " + fps + L" Z:" + zoomText;
        mapTest.drawTopText(shadersBucket, mapSymbols, mapCamera, textInfo, 0.5f, 0.2f);

        std::wstring latLonText =  L" Lat: " + Utils::floatToWString(RAD2DEG(mn.camLatitude), 4) +
                                   L" Lon: " + Utils::floatToWString(RAD2DEG(mn.camLongitude), 4);

        mapTest.drawTopText(shadersBucket, mapSymbols, mapCamera, latLonText, 0.5f, 0.25f);
    }


//    mapTest.drawCenterPoint(shadersBucket, pvFloat);
//    mapTest.drawTextureTest(shadersBucket, mapCamera, mapTileRender.getMapTexture(), 2, 2);
    //drawTestTexture(markers.nextPlaceForAvatar.atlasId, 0.8, 0.8, 0.2);

    //drawTestTexture(mapTileRender.getMapTexture(), 0.8, 0.8, 0.2);
}

void MapRenderer::init(AAssetManager *assetManager, JNIEnv *env, jobject &request_tile) {
    mapSymbols.loadFont(assetManager);
    mapTileGetter = new MapTileGetter(env, request_tile, mapSymbols);
    mapTileGetter->getOrRequest(0, 0, 0);
}

void MapRenderer::onSurfaceChanged(int screenW, int screenH) {
    mapCamera = MapCamera(screenW, screenH, 60.0f);
    mapTest.init(mapCamera);
    mapTileRender.screenWidth = screenW;
    mapTileRender.screenHeight = screenH;
}

void MapRenderer::onSurfaceCreated(AAssetManager *assetManager) {
    if (surfaceCreated) return; // если уже все что нужно для карты создано то не создаем заново

    auto error = CommonUtils::getGLErrorString();
    shadersBucket.compileAllShaders(assetManager);
    mapSymbols.initGl(assetManager, mapCamera, shadersBucket);
    mapTileRender.initTilesTexture();
    mapEnvironment.init(planeSize);
    drawMap.onSurfaceCreated();
    markers.initGL();
    surfaceCreated = true;

    int maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    error = CommonUtils::getGLErrorString();

    float lat = DEG2RAD(55.7486);
    float lon = DEG2RAD(37.6191);
    //animateCameraTo.addAnimation(0, moscowLat, moscowLon, 2);
    //animateCameraTo.addAnimation(17, moscowLat, moscowLon, 1);
    mapControls.setCamPos(lat, lon);
    mapControls.setZoom(13.1);

    //mapControls.setCamPos(DEG2RAD(-23.5808), DEG2RAD(-46.6698));
    //mapControls.setZoom(11.4);

    transferTilesToGpu.createdSurfaceCount++;
}

void MapRenderer::drag(float dx, float dy) {
    mapControls.drag(dx, dy);
}

float MapRenderer::scale(float scaleFactor) {
    mapControls.scale(scaleFactor);
    return mapControls.getZoom();
}

void MapRenderer::doubleTap() {
    mapControls.doubleTap();
    markers.doubleTap();
}

MapRenderer::MapRenderer() {
    mapControls.initCamUnit(planeSize);
}

 void MapRenderer::drawTestTexture(GLuint textureId, float width, float height, float shift) {
     auto textureShader = shadersBucket.textureShader;
     float ratio = mapCamera.getRatio();
     std::vector<float> testVertices = {
             -ratio + shift,         -1.0f + shift,
             -ratio + shift + width, -1.0f + shift,
             -ratio + shift + width, -1.0f + shift + height,
             -ratio + shift,         -1.0f + shift + height
     };
     std::vector<float> uvTest = {
             0, 0,
             1, 0,
             1, 1,
             0, 1
     };
     Eigen::Matrix4f projectionTest = mapCamera.createOrthoProjection(-ratio, ratio, -1, 1, 0.1, 1);
     Eigen::Matrix4f viewTest = mapCamera.createView(0, 0, 1, 0, 0, 0);
     Eigen::Matrix4f pvTest = projectionTest * viewTest;
     glUseProgram(textureShader->program);
     glUniformMatrix4fv(textureShader->getMatrixLocation(), 1, GL_FALSE, pvTest.data());
     glVertexAttribPointer(textureShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, testVertices.data());
     glEnableVertexAttribArray(textureShader->getPosLocation());
     glVertexAttribPointer(textureShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, uvTest.data());
     glEnableVertexAttribArray(textureShader->getTextureCord());
     glBindTexture(GL_TEXTURE_2D, textureId);
     glUniform1f(textureShader->getTextureLocation(), 0);
     glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
 }

 int64_t MapRenderer::confirmedClick(float x, float y, int64_t ignore) {
    return markers.confirmedClick(x, y, ignore);
}

void MapRenderer::cleanup() {
    markers.joinThreads();
    mapTileGetter->joinThreads();
    mapTileRender.joinThreads();
    delete mapTileGetter;
}

void MapRenderer::deselectSelectedMarker() {
    markers.deselectSelectedMarker();
}

void MapRenderer::destroyGL() {
    markers.destroy();
    mapTileRender.destroy();
    mapEnvironment.destroy();
    mapSymbols.destroy();
    shadersBucket.destroy();
    mapTileGetter->destroy();
    drawMap.destroy();
}





