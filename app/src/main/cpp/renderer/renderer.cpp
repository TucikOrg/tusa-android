//
// Created by Artem on 02.01.2024.
//

#include "renderer/renderer.h"
#include <GLES2/gl2.h>
#include "util/matrices.h"
#include "util/frustrums.h"
#include <thread>
#include <mapbox/earcut.hpp>
#include "util/android_log.h"
#include "shader/planet_shader.h"
#include "util/eigen_gl.h"
#include "renderer/render_tile_hash.h"
#include "renderer/visible_tiles_result.h"
#include <cmath>
#include <iostream>
#include <thread>
#include <stack>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/StdVector>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool DEBUG = false;
bool DRAW_DEBUG_UI = false;

std::atomic<bool> networkTileThreadRunning;
std::mutex networkTileStackMutex;
std::mutex insertToTreeMutex;

Renderer::Renderer(
        std::shared_ptr<ShadersBucket> shadersBucket,
        Cache* cache
):
    shadersBucket(shadersBucket),
    cache(cache) {
    symbols = std::shared_ptr<Symbols>(new Symbols(shadersBucket));
    renderTileCoordinates = std::shared_ptr<RenderTileCoordinates>(new RenderTileCoordinates(shadersBucket, symbols));
}


Eigen::Matrix4f Renderer::evaluatePVM() {
    float targetX = 0;
    float targetY = 0;
    float targetZ = 0;

    if (flatRender) {
        camX = cameraCurrentDistance;
        camY = cameraY;
        camZ = cameraZ;

        targetY = cameraY;
        targetZ = cameraZ;
    } else {
        camX = cameraCurrentDistance * cos(cameraLatitudeRad) * cos(cameraLongitudeRad);
        camY = cameraCurrentDistance * sin(cameraLatitudeRad);
        camZ = cameraCurrentDistance * cos(cameraLatitudeRad) * sin(cameraLongitudeRad);
    }

    Eigen::Vector3f cameraPosition = Eigen::Vector3f(camX, camY, camZ);
    Eigen::Vector3f target(targetX, targetY, targetZ);
    Eigen::Vector3f up(0.0f, 1.0f, 0.0f);
    Eigen::Matrix4f viewMatrix = EigenGL::createViewMatrix(cameraPosition, target, up);
    Eigen::Matrix4f pvm = projectionMatrix * viewMatrix;
    return pvm;
}

void Renderer::renderFrame() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> frameDeltaTime = currentTime - previousRenderFrameTime;
    previousRenderFrameTime = currentTime;
    frameCount++;

    updateCameraPosition(flatRender);
    updateFrustum(flatRender);

    Eigen::Matrix4f pvm = evaluatePVM();
    bool refreshTexture = false;
    std::chrono::duration<float> fpsTimeSpan = currentTime - fpsRenderFrameTime;
    if (fpsTimeSpan.count() >= 1.0f) {
        fps = frameCount / fpsTimeSpan.count();
        frameCount = 0;
        fpsRenderFrameTime = currentTime;
        LOGI("FPS: %f", fps);
    }

    std::vector<TileCords> renderFirstTiles = {};
    std::vector<TileCords> renderSecondTiles = {};

    std::chrono::duration<float> frameStateTimeSpan = currentTime - updateFrameStateTime;
    if (frameStateTimeSpan.count() >= refreshRenderDataEverySeconds && !DEBUG) {
        if (switchFlatSphereModeFlag) {
            switchFlatAndSphereRender();
            markersOnChangeRenderMode();
            pvm = evaluatePVM();
        }
        currentCornersCords = evaluateCorners(pvm);

        if (flatRender) {
            visibleTilesResult = updateVisibleTilesFlat(currentCornersCords);
        } else {
            visibleTilesResult = updateVisibleTilesSphere(currentCornersCords);
        }
        auto visibleTiles = visibleTilesResult.newVisibleTiles;
        updatePlanetGeometry(visibleTilesResult);

        std::string newRenderFrameKey = "";
        tilesForRenderer.clear();
        if (readyTilesTree != nullptr) {
            for (auto& visibleTile : visibleTiles) {
                bool isReplacement = false;
                auto foundTile = readyTilesTree->search(visibleTile.second, isReplacement, nullptr);
                if (foundTile->containsData() && tilesForRenderer.find(foundTile->tile.getKey()) == tilesForRenderer.end()) {
                    tilesForRenderer[foundTile->tile.getKey()] = foundTile->tile;
                    newRenderFrameKey += foundTile->tile.getKey();
                    if (isReplacement) {
                        renderFirstTiles.push_back(foundTile->tile);
                    } else {
                        renderSecondTiles.push_back(foundTile->tile);
                    }
                }
            }
        }

        newRenderFrameKey += std::to_string(realMapZTile());
        if (newRenderFrameKey != renderFrameKey) {
            refreshTexture = true;
            renderFrameKey = newRenderFrameKey;
        }
        updateFrameStateTime = currentTime;
    }

    float decayParameter = 0.95;
    if (!flatRender) {
        cameraLongitudeRad -= longitudeCameraMoveCurrentForce * frameDeltaTime.count();
        cameraLatitudeRad -= latitudeCameraMoveCurrentForce * frameDeltaTime.count();
        cameraLatitudeRad = std::max(-latitudeCameraAngleRadConstraint, std::min(cameraLatitudeRad, latitudeCameraAngleRadConstraint));
        LOGI("Camera latitude %f", cameraLatitudeRad);
    }

    longitudeCameraMoveCurrentForce *= decayParameter;
    latitudeCameraMoveCurrentForce *= decayParameter;

    std::shared_ptr<PlainShader> plainShader = shadersBucket->plainShader;
    std::shared_ptr<SymbolShader> symbolShader = shadersBucket->symbolShader;
    std::shared_ptr<PlanetShader> planetShader = shadersBucket->planetShader;

    if (refreshTexture) {
        Eigen::Matrix4f viewMatrixTexture = Eigen::Matrix4f::Identity();
        Eigen::Affine3f viewTranslation(Eigen::Translation3f(0, 0, -1));
        viewMatrixTexture *= viewTranslation.matrix();
        Eigen::Matrix4f pvmTexture = rendererTileProjectionMatrix * viewMatrixTexture;

        glBindTexture(GL_TEXTURE_2D, renderMapTexture);
        glBindFramebuffer(GL_FRAMEBUFFER, renderMapFrameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderMapTexture, 0);
        glViewport(0, 0, renderMapTextureWidth, renderMapTextureHeight);
        glClearColor((float)242 / 255, (float)248 / 255, (float)230 / 255, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        renderTileHash.clear();
        renderTiles(renderFirstTiles, pvmTexture);
        renderTiles(renderSecondTiles, pvmTexture);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    clearColor();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewPortDefaultSize();

    if (flatRender) {
        drawFlat(pvm);
    } else {
        drawPlanet(pvm);
    }

    std::shared_ptr<UserMarkerShader> userMarkerShader = shadersBucket->userMarkerShader;
    for (auto& marker : userMarkers) {
        marker.draw(pvm, userMarkerShader,
                    testAvatarTextureId,
                    cameraLongitudeRad, cameraLatitudeRad,
                    getSphereLonRad(), getSphereLatRad(),
                    getCameraPosition(), flatRender
                    );
    }

    if (DRAW_DEBUG_UI) {
        auto pvm_UI = evaluatePVM_UI();
        drawTilesRenderTexture_UI(pvm_UI);
        drawFPS_UI(pvm_UI);
    }

    if (DEBUG) {
        drawPoints(pvm, planetGeometry.sphere_vertices, 10.0f);

        if (flatRender) {
            drawPoint(pvm, 0, cameraY, cameraZ, 30.0f);
        } else {
            drawPoint(pvm, 0, 0, 0, 30.0f);
        }
    }
}

void Renderer::drawPlanet(Eigen::Matrix4f pvm) {
    if (realMapZTile() < drawStarsToZoom) {
        drawStars(pvm);
        drawGlowing(pvm);
    }

    // 1_drawPlanet
    glStencilMask(0X00);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);

    std::shared_ptr<PlanetShader> planetShader = shadersBucket->planetShader;
    std::shared_ptr<PlainShader> plainShader = shadersBucket->plainShader;
    glUseProgram(planetShader->program);
    glUniformMatrix4fv(planetShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());

    glVertexAttribPointer(planetShader->getUnitSquareCoordinates(), 2, GL_FLOAT, GL_FALSE, 0, planetGeometry.unitSquareCoordinates.data());
    glEnableVertexAttribArray(planetShader->getUnitSquareCoordinates());
    glVertexAttribPointer(planetShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, planetGeometry.sphere_vertices.data());
    glEnableVertexAttribArray(planetShader->getPosLocation());

    glBindTexture(GL_TEXTURE_2D, renderMapTexture);
    glUniform1i(planetShader->getTileTextureLocation0(), 0);
    glDrawElements(GL_TRIANGLES, planetGeometry.sphere_indices.size(), GL_UNSIGNED_INT, planetGeometry.sphere_indices.data());

    //drawPoints(pvm, planetGeometry.sphere_vertices, 10.0f);
}

void Renderer::drawFlat(Eigen::Matrix4f pvm) {
    // 1_drawPlanet
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);

    std::shared_ptr<PlanetShader> planetShader = shadersBucket->planetShader;
    std::shared_ptr<PlainShader> plainShader = shadersBucket->plainShader;
    glUseProgram(planetShader->program);

    glUniformMatrix4fv(planetShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());

    glVertexAttribPointer(planetShader->getUnitSquareCoordinates(), 2, GL_FLOAT, GL_FALSE, 0, planetGeometry.unitSquareCoordinates.data());
    glEnableVertexAttribArray(planetShader->getUnitSquareCoordinates());
    glVertexAttribPointer(planetShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, planetGeometry.sphere_vertices.data());
    glEnableVertexAttribArray(planetShader->getPosLocation());

    glBindTexture(GL_TEXTURE_2D, renderMapTexture);
    glUniform1i(planetShader->getTileTextureLocation0(), 0);
    glDrawElements(GL_TRIANGLES, planetGeometry.sphere_indices.size(), GL_UNSIGNED_INT, planetGeometry.sphere_indices.data());
}

void Renderer::onSurfaceChanged(int w, int h) {
    screenW = w;
    screenH = h;
    glViewPortDefaultSize();
    updateFrustum(flatRender);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &renderMapTextureWidth);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &renderMapTextureHeight);

    renderMapTextureWidth = 8000;
    renderMapTextureHeight = 8000;

    glBindTexture(GL_TEXTURE_2D, renderMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderMapTextureWidth, renderMapTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

VisibleTilesResult Renderer::updateVisibleTilesFlat(CornersCords corners) {
    short z = currentMapZTile();
    int n = pow(2, z);
    float leftTop_zTile = 0;
    float leftTop_yTile = 0;
    float rightBottom_zTile = n - 1;
    float rightBottom_yTile = n - 1;

    float tileSize = 2 * planetRadius / n;
    float leftTopZ_n = flatZNormalized(corners.leftTopZ);
    float leftTopY_n = flatYNormalized(corners.leftTopY);
    float rightBottomZ_n = flatZNormalized(corners.rightBottomZ);
    float rightBottomY_n = flatYNormalized(corners.rightBottomY);

    leftTop_zTile = abs(leftTopZ_n) / tileSize;
    leftTop_yTile = abs(leftTopY_n) / tileSize;
    rightBottom_zTile = abs(rightBottomZ_n) / tileSize;
    rightBottom_yTile = abs(rightBottomY_n) / tileSize;

    std::map<std::string, TileCords> newVisibleTiles = {};

    for (int zTile = (int) leftTop_zTile; zTile <= (int) rightBottom_zTile; zTile++) {
        for (int yTile = (int) leftTop_yTile; yTile <= (int) rightBottom_yTile; yTile++) {
            auto tileCord = TileCords(zTile, yTile, z);
            newVisibleTiles[tileCord.getKey()] = std::move(tileCord);
        }
    }

    VisibleTilesBlock visibleBlock = VisibleTilesBlock {
            (int)leftTop_zTile,
            (int)rightBottom_zTile,
            (int)leftTop_yTile,
            (int)rightBottom_yTile,
            z
    };

    int renderZDiffSize = (int) rightBottom_zTile - (int) leftTop_zTile + 1;
    int renderYDiffSize = (int) rightBottom_yTile - (int) leftTop_yTile + 1;

    updateRenderTileProjection(renderZDiffSize, renderYDiffSize);

    // end_updateVisibleTiles
    std::ostringstream oss;
    for (auto& tile : newVisibleTiles) {
        oss << tile.second.toString() << " ";
    }
    LOGI("[SHOW_PIPE] New visible tiles: %s", oss.str().c_str());
    LOGI("[VISIBLE_TILES_SIZE] %s", std::to_string(newVisibleTiles.size()).c_str());

    networkTileStackMutex.lock();
    for(auto& tilePair : newVisibleTiles) {
        auto& tile = tilePair.second;
        // Если тайлы уже в рендре или уже в видимых тайлах то нету смысла их грузить
        if(isInPreviousVisibleTiles(tile))
            continue;

        networkTilesStack.push(tile);
    }
    networkTileStackMutex.unlock();
    previousVisibleTiles = newVisibleTiles;
    return VisibleTilesResult {
            visibleBlock,
            newVisibleTiles
    };

}

VisibleTilesResult Renderer::updateVisibleTilesSphere(CornersCords cornersCords) {
    // 1_updateVisibleTiles

    short z = currentMapZTile();
    float n = pow(2, z);

    float leftTop_x = 0;
    float leftTop_y = 0;
    float leftTop_xTile = 0;
    float leftTop_yTile = 0;

    float rightBottom_x = 1;
    float rightBottom_y = 1;
    float rightBottom_xTile = n - 1;
    float rightBottom_yTile = n - 1;

    if (cornersCords.hasLeftTop) {
        leftTop_x = (cornersCords.leftTopLongitudeRad + M_PI) / (2 * M_PI);
        leftTop_y = CommonUtils::latitudeRadToY(-cornersCords.leftTopLatitudeRad);
        leftTop_xTile = leftTop_x * n;
        leftTop_yTile = leftTop_y * n;
    }

    if (cornersCords.hasRightBottom) {
        rightBottom_x = (CommonUtils::normalizeLongitudeRad(cornersCords.rightBottomLongitudeRad) + M_PI) / (2 * M_PI);
        rightBottom_y = CommonUtils::latitudeRadToY(-cornersCords.rightBottomLatitudeRad);
        rightBottom_xTile = rightBottom_x * n;
        rightBottom_yTile = rightBottom_y * n;

        float rightTop_x = (CommonUtils::normalizeLongitudeRad(cornersCords.topRightLongitudeRad) + M_PI) / (2 * M_PI);
        float rightTop_xTile = rightTop_x * n;
        if (rightTop_xTile > rightBottom_xTile || rightTop_xTile < leftTop_xTile) {
            rightBottom_xTile = rightTop_xTile;
        }
    }


    std::map<std::string, TileCords> newVisibleTiles = {};
    bool isOnTheEdge = leftTop_xTile > rightBottom_xTile;
    if (isOnTheEdge) {
        // Значит мы на стыке карты
        // то есть мы подошли к краю карты и видим начало следующего лупа карты.
        for (int xTile = (int)leftTop_xTile; xTile < (int)n; xTile++) {
            for (int yTile = (int)leftTop_yTile; yTile <= (int)rightBottom_yTile; yTile++) {
                TileCords tileCord = TileCords(xTile, yTile, z);
                newVisibleTiles[tileCord.getKey()] = std::move(tileCord);
            }
        }

        for (int xTile = (int)0; xTile <= (int)rightBottom_xTile; xTile++) {
            for (int yTile = (int)leftTop_yTile; yTile <= (int)rightBottom_yTile; yTile++) {
                auto tileCord = TileCords(xTile, yTile, z);
                newVisibleTiles[tileCord.getKey()] = std::move(tileCord);
            }
        }
    } else {
        for (int xTile = (int)leftTop_xTile; xTile <= (int)rightBottom_xTile; xTile++) {
            for (int yTile = (int)leftTop_yTile; yTile <= (int)rightBottom_yTile; yTile++) {
                auto tileCord = TileCords(xTile, yTile, z);
                newVisibleTiles[tileCord.getKey()] = std::move(tileCord);
            }
        }
    }
    VisibleTilesBlock visibleBlocks = VisibleTilesBlock {
            (int)leftTop_xTile,
            (int)rightBottom_xTile,
            (int)leftTop_yTile,
            (int)rightBottom_yTile,
            z
    };

    int renderXDiffSize = (int) rightBottom_xTile - (int) leftTop_xTile + 1;
    int renderYDiffSize = (int) rightBottom_yTile - (int) leftTop_yTile + 1;
    if (isOnTheEdge) {
        renderXDiffSize = (int) n - (int) leftTop_xTile + (int) rightBottom_xTile + 1;
    }

    updateRenderTileProjection(renderXDiffSize, renderYDiffSize);

    // end_updateVisibleTiles
    std::ostringstream oss;
    for (auto& tile : newVisibleTiles) {
        oss << tile.second.toString() << " ";
    }
    LOGI("[SHOW_PIPE] New visible tiles: %s", oss.str().c_str());
    LOGI("[VISIBLE_TILES_SIZE] %s", std::to_string(newVisibleTiles.size()).c_str());

    networkTileStackMutex.lock();
    for(auto& tilePair : newVisibleTiles) {
        auto& tile = tilePair.second;
        // Если тайлы уже в рендре или уже в видимых тайлах то нету смысла их грузить
        if(isInPreviousVisibleTiles(tile))
            continue;

        networkTilesStack.push(tile);
    }
    networkTileStackMutex.unlock();
    previousVisibleTiles = newVisibleTiles;
    return VisibleTilesResult {
            visibleBlocks,
            newVisibleTiles
    };
}


void Renderer::drag(float dx, float dy) {
    LOGI("Planet coordinates: Latitude: %f Longitude %f", getPlanetCurrentLatitudeDeg(),
         getPlanetCurrentLongitudeDeg());

    double scale = pow(2, scaleFactorZoom);

    if (flatRender) {
        cameraZ -= dx * dragCameraSpeedFlatMap / scale;
        cameraY -= dy * dragCameraSpeedFlatMap / scale;
    } else {
        longitudeCameraMoveCurrentForce += dx * forceCameraMoveOnDragSphere / scale;
        latitudeCameraMoveCurrentForce += dy * forceCameraMoveOnDragSphere / scale;
    }
}

void Renderer::scale(float factor) {
    LOGI("Factor %f", factor);
    updateMapZoomScaleFactor(factor); // обновляет текущий скейл, параметр скейла
    LOGI("Scale factor %f", scaleFactorZoom);

    latitudeCameraMoveCurrentForce = 0;
    longitudeCameraMoveCurrentForce = 0;

    if(_savedLastScaleStateMapZ != realMapZTile()) {
        updateMarkersSizes();
        _savedLastScaleStateMapZ = realMapZTile();
        if (
                (_savedLastScaleStateMapZ >= switchFlatSphereOnZoom && !flatRender) ||
                (_savedLastScaleStateMapZ < switchFlatSphereOnZoom && flatRender)) {
            switchFlatSphereModeFlag = true;

        }
        LOGI("New Z of map %d", realMapZTile());
        LOGI("Current tile extent %f", evaluateCurrentExtent());
    }
}

void Renderer::doubleTap() {
    //DEBUG = !DEBUG;
    //switchFlatSphereModeFlag = true;
}

void Renderer::networkTilesFunction(JavaVM* gJvm, GetTileRequest* getTileRequest) {
    JNIEnv* threadEnv;
    gJvm->AttachCurrentThread(&threadEnv, NULL);
    getTileRequest->setEnv(threadEnv);

    while (networkTileThreadRunning.load()) {
        networkTileStackMutex.lock();
        TileCords tileToNetwork;
        bool showCord = false;
        if (!networkTilesStack.empty()) {
            tileToNetwork = networkTilesStack.top();
            networkTilesStack.pop();
            showCord = true;
        }
        networkTileStackMutex.unlock();

        if (showCord) {
            auto drawGeometry = tilesStorage.getTile(tileToNetwork.getTileZ(), tileToNetwork.getTileX(), tileToNetwork.getTileY(), getTileRequest);
            tileToNetwork.setData(drawGeometry);
            insertToTreeMutex.lock();
            if (readyTilesTree == nullptr) {
                readyTilesTree = new TileNode(tileToNetwork);
            } else {
                bool exists;
                auto inserted = readyTilesTree->insert(tileToNetwork, exists);
            }
            insertToTreeMutex.unlock();
        }
    }

    gJvm->DetachCurrentThread();
}

void Renderer::setupNoOpenGLMapState(float scaleFactor, AAssetManager *assetManager, JNIEnv *env) {
    loadAssets(assetManager);
    updateMapZoomScaleFactor(scaleFactor);
    updateCameraPosition(flatRender);
    _savedLastScaleStateMapZ = realMapZTile();
    JavaVM* gJvm = nullptr;
    env->GetJavaVM(&gJvm);
    GetTileRequest* getTileRequest = new GetTileRequest(cache, env);

    // network_tile_thread
    networkTileThreadRunning.store(true);
    for(short i = 0; i < networkTilesThreads; i++) {
        std::thread networkTileThread(std::bind(&Renderer::networkTilesFunction, this, gJvm, getTileRequest));
        networkTileThread.detach();
        networkTileThreads.push_back(&networkTileThread);
    }
}

void Renderer::loadAssets(AAssetManager *assetManager) {
    if(assetManager == nullptr)
        return;
    symbols->loadFont(assetManager);
}

void Renderer::updateFrustum(bool flatRender) {
    // сколько нужно проскролить до нового зума
    float currentCameraDistance = evaluateCameraDistanceCurrentZ();
    float nextCameraDistance = evaluateCameraDistanceCurrentZ(1);
    float distanceDelta = currentCameraDistance - nextCameraDistance;

    if (flatRender) {
        float near = currentCameraDistance - distanceDelta;
        if (near < 2) {
            near = 0.1;
        }
        float far = currentCameraDistance;
        projectionMatrix = EigenGL::createPerspectiveProjectionMatrix(
                fovy,
                (float) screenW / (float) screenH,
                near,
                far
        );
    } else {
        float near = currentCameraDistance - planetRadius - distanceDelta;
        float far = currentCameraDistance + planetRadius * 10;
        projectionMatrix = EigenGL::createPerspectiveProjectionMatrix(
                fovy,
                (float) screenW / (float) screenH,
                near,
                far
        );
    }
}

void Renderer::loadTextures(AAssetManager *assetManager) {
    loadTextureFromAsset(testAvatarTextureId, "images/artem.jpg", assetManager);
}

void Renderer::loadTextureFromAsset(unsigned int &textureId, const char *assetName,
                                    AAssetManager *assetManager) {
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    AAsset* asset = AAssetManager_open(assetManager, assetName, AASSET_MODE_UNKNOWN);
    off_t fileSize = AAsset_getLength(asset);
    unsigned char* imageData = (unsigned char*) malloc(fileSize);
    AAsset_read(asset, imageData, fileSize);
    AAsset_close(asset);

    int width, height, channels;
    unsigned char* image = stbi_load_from_memory(imageData, fileSize, &width, &height, &channels, STBI_rgb);

    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            width,
            height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            image
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(imageData);
    delete[] image;
}

void Renderer::onSurfaceCreated(AAssetManager *assetManager) {
    getSymbols()->createFontTextures();
    loadTextures(assetManager);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_STENCIL_TEST);

    glGenFramebuffers(1, &renderMapFrameBuffer);
    glGenTextures(1, &renderMapTexture);

    generateStarsGeometry();

    auto marker = UserMarker(
            DEG2RAD(55.7558),
            DEG2RAD(37.6176),
            planetRadius,
            false, evaluateInitMarkerSize()
    );
    userMarkers.push_back(marker);
}

void Renderer::updateRenderTileProjection(short amountX, short amountY) {
    rendererTileProjectionMatrix = EigenGL::createOrthoMatrix(0, 4096 * amountX, -4096 * amountY, 0, 0.1, 100);
}

CornersCords Renderer::evaluateCorners(Eigen::Matrix4f pvm) {
    std::vector<Eigen::Vector4f> planes = EigenGL::extractFrustumPlanes(pvm);
    auto leftPlane = planes[0];
    auto topPlane = planes[3];
    double topLeftLongitudeRad = 0;
    double topLeftLatitudeRad = 0;
    bool hasTopLeft = false;

    auto rightPlane = planes[1];
    auto bottomPlane = planes[2];
    double bottomRightLongitudeRad = 0;
    double bottomRightLatitudeRad = 0;
    bool hasBottomRight = false;

    double topRightLongitudeRad = 0;
    double topRightLatitudeRad = 0;
    bool hasTopRight = false;

    float leftTop_y = 0;
    float leftTop_z = 0;

    float rightBottom_y = 0;
    float rightBottom_z = 0;

    if (flatRender) {
        evaluateLatLonByIntersectionPlanes_Flat(
                leftPlane,
                topPlane,
                pvm,
                leftTop_y,
                leftTop_z
        );

        evaluateLatLonByIntersectionPlanes_Flat(
                rightPlane,
                bottomPlane,
                pvm,
                rightBottom_y,
                rightBottom_z
        );
    } else {
        evaluateLatLonByIntersectionPlanes_Sphere(
                leftPlane,
                topPlane,
                pvm,
                topLeftLongitudeRad,
                topLeftLatitudeRad,
                hasTopLeft
        );

        evaluateLatLonByIntersectionPlanes_Sphere(
                rightPlane,
                bottomPlane,
                pvm,
                bottomRightLongitudeRad,
              bottomRightLatitudeRad,
              hasBottomRight
        );

        evaluateLatLonByIntersectionPlanes_Sphere(
                topPlane,
              rightPlane,
                   pvm,
                topRightLongitudeRad,
                topRightLatitudeRad,
                hasTopRight
        );
    }

    return CornersCords {
            topLeftLongitudeRad,
            topLeftLatitudeRad,
            bottomRightLongitudeRad,
            bottomRightLatitudeRad,
            hasTopLeft,
            hasBottomRight,
            topRightLongitudeRad,
            topRightLatitudeRad,
            hasTopRight,
            leftTop_y,
            leftTop_z,
            rightBottom_y,
            rightBottom_z
    };
}

void Renderer::evaluateLatLonByIntersectionPlanes_Flat(Eigen::Vector4f firstPlane, Eigen::Vector4f secondPlane, Eigen::Matrix4f pvm, float& y, float& z) {
    double B1 = firstPlane[1];
    double C1 = firstPlane[2];
    double D1 = firstPlane[3];

    double B2 = secondPlane[1];
    double C2 = secondPlane[2];
    double D2 = secondPlane[3];

    double det = B1 * C2 - B2 * C1;
    double detY = (-D1) * C2 - (-D2) * C1;
    double detZ = B1 * (-D2) - B2 * (-D1);

    double y0 = detY / det;
    double z0 = detZ / det;
    double x0 = 0;

    y = y0;
    z = z0;
}

void Renderer::evaluateLatLonByIntersectionPlanes_Sphere(
        Eigen::Vector4f firstPlane, Eigen::Vector4f secondPlane,
        Eigen::Matrix4f pvm, double& longitudeRad, double& latitudeRad, bool& has) {
    double A1 = firstPlane[0];
    double B1 = firstPlane[1];
    double C1 = firstPlane[2];
    double D1 = firstPlane[3];

    double A2 = secondPlane[0];
    double B2 = secondPlane[1];
    double C2 = secondPlane[2];
    double D2 = secondPlane[3];

    double leftTopPlaneLineVector[3] = {
            B1 * C2 - C1 * B2, // a
            C1 * A2 - A1 * C2, // b
            A1 * B2 - B1 * A2 // c
    };

    double det = A1 * B2 - A2 * B1;
    double detX = (-D1) * B2 - (-D2) * B1;
    double detY = A1 * (-D2) - A2 * (-D1);

    double x0 = detX / det;
    double y0 = detY / det;
    double z0 = 0; // потому что мы взяли его как свободный параметр для нахождения любой точки

    double a = leftTopPlaneLineVector[0];
    double b = leftTopPlaneLineVector[1];
    double c = leftTopPlaneLineVector[2];

    double Ay = pow(a, 2.0) + pow(b, 2.0) + pow(c, 2.0);
    double By = 2 * (x0 * a + y0 * b + z0 * c);
    double Cy = pow(x0, 2.0) + pow(y0, 2.0) + pow(z0, 2.0) - pow(planetRadius, 2.0);
    double discremenant = pow(By, 2.0) - 4 * Ay * Cy;
    if (discremenant > 0) {
        double t1 = (-By + sqrt(discremenant)) / (2 * Ay);
        double t2 = (-By - sqrt(discremenant)) / (2 * Ay);

        double xi1 = x0 + a * t1;
        double yi1 = y0 + b * t1;
        double zi1 = z0 + c * t1;

        // точка пересечения ближайшая к камере
        double xi2 = x0 + a * t2;
        double yi2 = y0 + b * t2;
        double zi2 = z0 + c * t2;

        longitudeRad = atan2(xi2, zi2) - M_PI / 2;
        longitudeRad = CommonUtils::normalizeLongitudeRad(longitudeRad);

        double hypotenuse = sqrt(pow(xi2, 2.0) + pow(zi2, 2.0));
        latitudeRad = atan2(yi2, hypotenuse);
        has = true;
        return;
    }

    longitudeRad = 0;
    latitudeRad = 0;
    has = false;
}

void Renderer::updatePlanetGeometry(VisibleTilesResult visibleTilesResult) {
    auto& block = visibleTilesResult.visibleBlocks;
    int tileX_start = block.tileX_start;
    int tileX_end = block.tileX_end;
    int tileY_start = block.tileY_start;
    int tileY_end = block.tileY_end;
    int zoom = block.zoom;

    int decreaseGrainSpeed = zoom / 2;
    int tileGrain = 34 / pow(2, decreaseGrainSpeed);

    if (tileGrain == 0) {
        tileGrain = 1;
    }
    if (zoom == 0) {
        tileGrain = 24;
    }
    if (zoom == 1) {
        tileGrain = 24;
    }

    if (flatRender) {
        planetGeometry.generateFlatData(tileX_start, tileX_end, tileY_start, tileY_end, zoom, tileGrain, planetRadius);
    } else {
        planetGeometry.generateSphereData(tileX_start, tileX_end, tileY_start, tileY_end, zoom, tileGrain, planetRadius);
    }
}

void Renderer::onStop() {
    // когда экран сворачивается то вызывается этот метод
    // и все крашиться :(
    // он должен запускаться только если приложение выключается
//    networkTileThreadRunning.store(false);
//    for (std::thread* networkTileThread : networkTileThreads) {
//        if (networkTileThread->joinable()) {
//            networkTileThread->join();
//        }
//    }
}

void Renderer::onDown() {
    latitudeCameraMoveCurrentForce = 0;
    longitudeCameraMoveCurrentForce = 0;
}

void Renderer::clearColor() {
    if (realMapZTile() < drawStarsToZoom) {
        // Цвет космоса
        CSSColorParser::Color colorOfStyle = spaceColor;
        GLfloat red   = static_cast<GLfloat>(colorOfStyle.r) / 255;
        GLfloat green = static_cast<GLfloat>(colorOfStyle.g) / 255;
        GLfloat blue  = static_cast<GLfloat>(colorOfStyle.b) / 255;
        GLfloat alpha = static_cast<GLfloat>(colorOfStyle.a);
        glClearColor(red, green, blue, alpha);
    } else {
        CSSColorParser::Color colorOfStyle = backgroundColor;
        GLfloat red   = static_cast<GLfloat>(colorOfStyle.r) / 255;
        GLfloat green = static_cast<GLfloat>(colorOfStyle.g) / 255;
        GLfloat blue  = static_cast<GLfloat>(colorOfStyle.b) / 255;
        GLfloat alpha = static_cast<GLfloat>(colorOfStyle.a);
        glClearColor(red, green, blue, alpha);
    }
}

void Renderer::generateStarsGeometry() {
    float from = -planetRadius * 10;
    float to = planetRadius * 10;
    float innerFrom = -planetRadius * 5;
    float innerTo = planetRadius * 5;
    starsVertices.clear();
    starsSizes.clear();
    for (int i = 0; i < 1000; i++) {
        float x = CommonUtils::randomFloatInRange(from, to);
        float y = CommonUtils::randomFloatInRange(from, to);
        float z = CommonUtils::randomFloatInRange(from, to);
        if (x > innerFrom && x < innerTo && y > innerFrom && y < innerTo && z > innerFrom && z < innerTo) {
            continue;
        }
        starsVertices.push_back(x);
        starsVertices.push_back(y);
        starsVertices.push_back(z);
        starsSizes.push_back(CommonUtils::randomFloatInRange(2.0, 13.0));
    }
}

void Renderer::drawGlowing(Eigen::Matrix4f pvm) {
    glDisable(GL_DEPTH_TEST);
    auto planetGlowShader = shadersBucket->planetGlowShader;
    glUseProgram(planetGlowShader->program);
    Eigen::Matrix4f rotationY = EigenGL::createRotationMatrixY(-cameraLongitudeRad);
    Eigen::AngleAxisf rotationYBy90(-M_PI / 2, Eigen::Vector3f::UnitY());

    Eigen::Vector3f cameraVectorXZ(camX, 0, camZ);
    Eigen::Vector3f rotationUpDownAxis = rotationYBy90 * cameraVectorXZ;
    rotationUpDownAxis.normalize();
    Eigen::Matrix4f rotationUpDown = EigenGL::createRotationMatrixAxis(cameraLatitudeRad, rotationUpDownAxis);

    Eigen::Matrix4f rotated = pvm * rotationUpDown * rotationY;
    glUniformMatrix4fv(planetGlowShader->getMatrixLocation(), 1, GL_FALSE, rotated.data());
    glUniform4f(planetGlowShader->getColorLocation(), 1.0, 0.0, 0.0f, 1.0f);
    glUniform1f(planetGlowShader->getPointSizeLocation(), 900);
    float planetEffectRadius = planetRadius;
    float textureCords[] = {
            -1.0f, -1.0f,
            1.0f, -1.0f,
            1.0f, 1.0f,
            -1.0f, 1.0f
    };
    float effectRadius = planetEffectRadius * 2.0;
    glUniform1f(planetGlowShader->getRadiusLocation(), planetEffectRadius / effectRadius);
    float vertices[] = {
            0, -effectRadius, effectRadius,
            0, -effectRadius, -effectRadius,
            0, effectRadius, -effectRadius,
            0, effectRadius, effectRadius
    };
    glVertexAttribPointer(planetGlowShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(planetGlowShader->getPosLocation());

    glVertexAttribPointer(planetGlowShader->getTextureCoordinatesLocation(), 2, GL_FLOAT, GL_FALSE, 0, textureCords);
    glEnableVertexAttribArray(planetGlowShader->getTextureCoordinatesLocation());

    unsigned int indices[6] = {
            0, 1, 2,
            2, 3, 0
    };
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
}

void Renderer::drawStars(Eigen::Matrix4f pvm) {
    glEnable(GL_DEPTH_TEST);
    auto starsShader = shadersBucket.get()->starsShader;
    auto starsColor = CSSColorParser::parse("rgb(255, 242, 214)");
    GLfloat red   = static_cast<GLfloat>(starsColor.r) / 255;
    GLfloat green = static_cast<GLfloat>(starsColor.g) / 255;
    GLfloat blue  = static_cast<GLfloat>(starsColor.b) / 255;
    GLfloat alpha = static_cast<GLfloat>(starsColor.a);
    const GLfloat color[] = { red, green, blue, alpha};
    glUseProgram(starsShader->program);
    glUniform4fv(starsShader->getColorLocation(), 1, color);
    glUniformMatrix4fv(starsShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
    glVertexAttribPointer(starsShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, starsVertices.data());
    glEnableVertexAttribArray(starsShader->getPosLocation());
    auto error1 = CommonUtils::getGLErrorString();
    glVertexAttribPointer(starsShader->getPointSizeLocation(), 1, GL_FLOAT, GL_FALSE, 0, starsSizes.data());
    glEnableVertexAttribArray(starsShader->getPointSizeLocation());
    auto error = CommonUtils::getGLErrorString();
    glDrawArrays(GL_POINTS, 0, starsVertices.size() / 3);
}



