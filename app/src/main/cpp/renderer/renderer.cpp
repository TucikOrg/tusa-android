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
bool DRAW_DEBUG_UI = true;
bool DRAW_CENTER_POINT = true;

std::atomic<bool> networkTileThreadRunning;
std::mutex networkTileStackMutex;
std::mutex visibleTilesResultMutex;

Renderer::Renderer(
        std::shared_ptr<ShadersBucket> shadersBucket,
        Cache* cache
):
    shadersBucket(shadersBucket),
    cache(cache) {
    symbols = std::shared_ptr<Symbols>(new Symbols(shadersBucket));
    renderTileCoordinates = std::shared_ptr<RenderTileCoordinates>(new RenderTileCoordinates(shadersBucket, symbols));
}

void Renderer::evaluateCameraWorldPosition(bool isFlat, float& camWorldX, float& camWorldY, float& camWorldZ) {
    if (isFlat) {
        camWorldX = cameraCurrentDistance;
        camWorldY = cameraY;
        camWorldZ = cameraZ;
        return;
    }

    camWorldX = cameraCurrentDistance * cos(cameraLatitudeRad) * cos(cameraLongitudeRad);
    camWorldY = cameraCurrentDistance * sin(cameraLatitudeRad);
    camWorldZ = cameraCurrentDistance * cos(cameraLatitudeRad) * sin(cameraLongitudeRad);
}

Eigen::Matrix4f Renderer::evaluatePVM(bool isFlat, float camWorldX, float camWorldY, float camWorldZ) {
    float targetX = 0;
    float targetY = 0;
    float targetZ = 0;

    if (isFlat) {
        targetY = cameraY;
        targetZ = cameraZ;
    }

    Eigen::Vector3f cameraPosition = Eigen::Vector3f(camWorldX, camWorldY, camWorldZ);
    Eigen::Vector3f target(targetX, targetY, targetZ);
    Eigen::Vector3f up(0.0f, 1.0f, 0.0f);
    Eigen::Matrix4f viewMatrix = EigenGL::createViewMatrix(cameraPosition, target, up);
    Eigen::Matrix4f pvm = projectionMatrix * viewMatrix;
    return pvm;
}

VisibleTilesResult Renderer::evaluateVisibleTiles(CornersCords& corners, bool isFlat) {
    if (isFlat) {
        return updateVisibleTilesFlat(corners);
    }
    return updateVisibleTilesSphere(corners);
}

void Renderer::renderFrame() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> frameDeltaTime = currentTime - previousRenderFrameTime;
    previousRenderFrameTime = currentTime;
    frameCount++;

    updateCameraPosition();
    updateFrustum();

    evaluateCameraWorldPosition(flatRender, camWorldX, camWorldY, camWorldZ);
    Eigen::Matrix4f pvm = evaluatePVM(flatRender, camWorldX, camWorldY, camWorldZ);
    bool refreshTilesTexture = false;
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

    // Обновляем состояние карты
    // По этому состоянию рендриться картинка
    if (frameStateTimeSpan.count() >= refreshRenderDataEverySeconds && !DEBUG) {
        if (switchFlatSphereModeFlag) {
            switchFlatAndSphereRender(); // меняет flatRender флаг
            markersOnChangeRenderMode();
            evaluateCameraWorldPosition(flatRender, camWorldX, camWorldY, camWorldZ);
            pvm = evaluatePVM(flatRender, camWorldX, camWorldY, camWorldZ);
        }

        currentCornersCords = evaluateCorners(pvm, flatRender);
        visibleTilesResultMutex.lock();
        visibleTilesResult = evaluateVisibleTiles(currentCornersCords, flatRender);
        visibleTilesResultMutex.unlock();

        updateRenderTileProjection(visibleTilesResult.renderZDiffSize, visibleTilesResult.renderYDiffSize);
        pushToNetworkTiles(visibleTilesResult.newVisibleTiles);

//        auto testCorners__ = evaluateCorners(pvm, !flatRender);
//        auto testVisibleTiles__ = evaluateVisibleTiles(testCorners__, !flatRender);


        // Геометрия генерируется четко по видимым тайлам
        // Текстура так же маппиться по геометрии видимых тайлов
        // Текстура должна точно соответствовать геометрии так как накладывается от края до края геометрии
        auto visibleTiles = visibleTilesResult.newVisibleTiles;
        updatePlanetGeometry(visibleTilesResult);

        // По ключу опредеяем нужно ли текстуру новую рендрить или нет
        // Если есть изменения в видимых тайлах то значит нужно все заново отрендрить
        std::string newVisibleTilesKey;
        // Это списко уже добавленных тайлов
        // Он нужен чтобы не было дубликатов
        std::map<std::string, void*> tilesKeysToRender = {};

        if (readyTilesTree != nullptr) {
            for (auto& visibleTile : visibleTiles) {
                newVisibleTilesKey += visibleTile.second.getKey();
                bool isReplacement = false;
                auto foundTile = readyTilesTree->searchAndCreate(visibleTile.second, isReplacement, nullptr, tilesStorage);

                if (foundTile->containsData() && tilesKeysToRender.find(foundTile->tile.getKey()) == tilesKeysToRender.end()) {
                    tilesKeysToRender[foundTile->tile.getKey()] = nullptr;
                    if (isReplacement) {
                        renderFirstTiles.push_back(foundTile->tile);
                    } else {
                        renderSecondTiles.push_back(foundTile->tile);
                        newVisibleTilesKey += "e";
                    }
                }
            }
        }

        // Если есть изменения в видимых тайлах то значит нужно все заново отрендрить
        if (newVisibleTilesKey != currentVisibleTilesKey) {
            refreshTilesTexture = true;
            currentVisibleTilesKey = newVisibleTilesKey;
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

    // Рендрим текстуру карты которую потом наложим на геометрию карты
    // Текстура обновляется только если есть изменения в видимых тайлах
    if (refreshTilesTexture) {
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
        renderTiles(renderFirstTiles, pvmTexture, visibleTilesResult);
        renderTiles(renderSecondTiles, pvmTexture, visibleTilesResult);
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
        marker.second.draw(pvm, userMarkerShader,
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
    }

    if (DRAW_CENTER_POINT) {
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
    updateFrustum();

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

    float tileSize = flatTileSizeInit / n;
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

    auto renderZDiffSize = (short)((int) rightBottom_zTile - (int) leftTop_zTile + 1);
    auto renderYDiffSize = (short)((int) rightBottom_yTile - (int) leftTop_yTile + 1);

    // end_updateVisibleTiles
    std::ostringstream oss;
    for (auto& tile : newVisibleTiles) {
        oss << tile.second.toString() << " ";
    }
    LOGI("[SHOW_PIPE] New visible tiles (FLAT): %s", oss.str().c_str());
    LOGI("[VISIBLE_TILES_SIZE] %s", std::to_string(newVisibleTiles.size()).c_str());


    return VisibleTilesResult {
            visibleBlock,
            newVisibleTiles,
            renderZDiffSize,
            renderYDiffSize
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
    auto visibleBlocks = VisibleTilesBlock {
            (int)leftTop_xTile,
            (int)rightBottom_xTile,
            (int)leftTop_yTile,
            (int)rightBottom_yTile,
            z
    };

    auto renderXDiffSize = (short)((int) rightBottom_xTile - (int) leftTop_xTile + 1);
    auto renderYDiffSize = (short)((int) rightBottom_yTile - (int) leftTop_yTile + 1);
    if (isOnTheEdge) {
        renderXDiffSize = (short)((int) n - (int) leftTop_xTile + (int) rightBottom_xTile + 1);
    }

    // end_updateVisibleTiles
    std::ostringstream oss;
    for (auto& tile : newVisibleTiles) {
        oss << tile.second.toString() << " ";
    }
    LOGI("[SHOW_PIPE] New visible tiles (SPHERE): %s", oss.str().c_str());
    LOGI("[VISIBLE_TILES_SIZE] %s", std::to_string(newVisibleTiles.size()).c_str());

    return VisibleTilesResult {
            visibleBlocks,
            newVisibleTiles,
            renderXDiffSize,
            renderYDiffSize
    };
}

void Renderer::pushToNetworkTiles(std::map<std::string, TileCords>& newVisibleTiles) {

    for(auto& tilePair : newVisibleTiles) {
        auto& tile = tilePair.second;

        // Этот тайл в предыдущей итерации грузили
        if (previousVisibleTile.find(tile.getKey()) != previousVisibleTile.end())
            continue;

        // Если тайлы уже загружены то нету смысла их грузить
        if(tilesStorage.existInMemory(tile.getTileZ(), tile.getTileX(), tile.getTileY()))
            continue;

        networkTileStackMutex.lock();
        networkTilesStack.push(tile);
        networkTileStackMutex.unlock();
    }

    previousVisibleTile = newVisibleTiles;
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
        bool sphere2FlatCondition = _savedLastScaleStateMapZ >= switchSphere2FlatAtZoom && !flatRender;
        bool flat2SphereCondition = _savedLastScaleStateMapZ < switchSphere2FlatAtZoom && flatRender;
        if (sphere2FlatCondition || flat2SphereCondition) {
            switchFlatSphereModeFlag = true;
        }
        LOGI("New Z of map %d", realMapZTile());
        LOGI("Current tile extent %f", evaluateCurrentExtent());
    }
}

void Renderer::doubleTap() {
    //DEBUG = !DEBUG;
    switchFlatSphereModeFlag = true;
    //DEBUG_TILES = !DEBUG_TILES;
}

void Renderer::networkRootTileFunction(JavaVM* gJvm, GetTileRequest* getTileRequest) {
    JNIEnv* threadEnv;
    gJvm->AttachCurrentThread(&threadEnv, NULL);
    getTileRequest->setEnv(threadEnv);

    TileCords rootTile = TileCords(0, 0, 0);
    auto rootTileData= tilesStorage.getOrLoad(rootTile.getTileZ(), rootTile.getTileX(), rootTile.getTileY(), getTileRequest);
    rootTile.setData(rootTileData);
    readyTilesTree = new TileNode(rootTile);
}

void Renderer::networkTilesFunction(JavaVM* gJvm, GetTileRequest* getTileRequest) {
    JNIEnv* threadEnv;
    gJvm->AttachCurrentThread(&threadEnv, NULL);
    getTileRequest->setEnv(threadEnv);

    while (networkTileThreadRunning.load()) {
        // Берем тайлы для загрузки из списка и грузим потом
        TileCords tileToNetwork;
        bool shouldLoadTile = false;
        networkTileStackMutex.lock();
        if (!networkTilesStack.empty()) {
            tileToNetwork = networkTilesStack.top();
            networkTilesStack.pop();
            shouldLoadTile = true;
        }
        networkTileStackMutex.unlock();

        // Проверяем что тайл актуален и есть смысл его загружать к данному моменту
        // в момент когда наступает его очередь загрузки может быть так что он уже не нужен
        visibleTilesResultMutex.lock();
        if (visibleTilesResult.newVisibleTiles.find(tileToNetwork.getKey()) == visibleTilesResult.newVisibleTiles.end()) {
            shouldLoadTile = false;
        }
        visibleTilesResultMutex.unlock();

        // Рутовый тайл тоже не грузим. Он грузится в другом потоке
        if (tileToNetwork.getTileZ() == 0 && tileToNetwork.getTileX() == 0 && tileToNetwork.getTileY() == 0) {
            shouldLoadTile = false;
        }

        if (shouldLoadTile) {
            // Загружаем тайл
            auto loadedTileData = tilesStorage.getOrLoad(tileToNetwork.getTileZ(), tileToNetwork.getTileX(), tileToNetwork.getTileY(), getTileRequest);
        }
    }

    gJvm->DetachCurrentThread();
}

float Renderer::evaluateScaleFactor(float z, float zoomDiff) {
    return pow(2, mapZTileCordMax - z - zoomDiff);
}

float Renderer::evaluateScaleFactorCurrentZ(short zoomDiff) {
    return evaluateScaleFactor(realMapZTile(), zoomDiff);
}

void Renderer::markersOnChangeRenderMode() {
    for(auto& marker : userMarkers) {
        marker.second.onChangeRenderMode(flatRender);
    }
}

void Renderer::updateMarkersSizes() {
    float newScale = 1 / pow(2, realMapZTile());
    if (flatRender) {
        newScale *= 0.85;
    }
    for(auto& marker : userMarkers) {
        marker.second.animateToScale(newScale);
    }
}

void Renderer::updateCameraPosition() {
    cameraCurrentDistance = evaluateCameraDistance(scaleFactorZoom, 0);
    LOGI("Camera distance %f", cameraCurrentDistance);
}

short Renderer::currentMapZTile() {
    short realZ = realMapZTile();
    if (realZ > 16)
        realZ = 16;
    return realZ;
}

float Renderer::flatZNormalized(float z) {
    return fmod(z, flatHalfOfTileSizeInit) - flatHalfOfTileSizeInit;
}

float Renderer::flatYNormalized(float y) {
    return fmod(y, flatHalfOfTileSizeInit) - flatHalfOfTileSizeInit;
}

float Renderer::getFlatLongitude() {
    return -(cameraZ / flatHalfOfTileSizeInit) * M_PI;
}

float Renderer::getFlatLatitude() {
    float y = ((cameraY / flatHalfOfTileSizeInit) + 1) / 2;
    return CommonUtils::tileLatitude(1 - y, 1);
}

void Renderer::switchFlatAndSphereRender() {
    short z = currentMapZTile();
    int n = pow(2, z);

    if (flatRender) {
        float flatLongitude = getFlatLongitude();
        float flatLatitude = getFlatLatitude();

        cameraLongitudeRad = -flatLongitude;
        cameraLatitudeRad = flatLatitude;
        LOGI("FLAT -> SPHERE lat(%f) lon(%f)", flatLatitude, flatLongitude);
    } else {
        // это сфера и хотим плоскость
        float longitudeRad = getSphereLonRad();
        float latitudeRad = getSphereLatRad();
        cameraZ = CommonUtils::longitudeToFlatCameraZ(longitudeRad, flatHalfOfTileSizeInit);
        cameraY = CommonUtils::latitudeToFlatCameraY(latitudeRad, flatHalfOfTileSizeInit);
        LOGI("SPHERE -> FLAT lat(%f) lon(%f)", latitudeRad, longitudeRad);
    }

    switchFlatSphereModeFlag = false;
    flatRender = !flatRender;

    updateCameraPosition();
    updateFrustum();
    //LOGI("Flat lon %f lat %f", RAD2DEG(flatLongitude), RAD2DEG(flatLatitude));
}

float Renderer::getSphereLonRad() {
    return CommonUtils::normalizeLongitudeRad(-cameraLongitudeRad);
}

void Renderer::drawPoints(Eigen::Matrix4f matrix, std::vector<float> points, float pointSize) {
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    auto plainShader = shadersBucket.get()->plainShader;
    const GLfloat color[] = { 1, 0, 0, 1};
    glUseProgram(plainShader->program);
    glUniform1f(plainShader->getPointSizeLocation(), pointSize);
    glUniform4fv(plainShader->getColorLocation(), 1, color);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, matrix.data());
    glVertexAttribPointer(plainShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, points.data());
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glDrawArrays(GL_POINTS, 0, points.size() / 3);
}

void Renderer::drawPoint(Eigen::Matrix4f matrix, float x, float y, float z, float pointSize) {
    float points[] = {x, y, z};
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    auto plainShader = shadersBucket.get()->plainShader;
    const GLfloat color[] = { 1, 0, 0, 1};
    glUseProgram(plainShader->program);
    glUniform1f(plainShader->getPointSizeLocation(), pointSize);
    glUniform4fv(plainShader->getColorLocation(), 1, color);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, matrix.data());
    glVertexAttribPointer(plainShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, points);
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glDrawArrays(GL_POINTS, 0, 1);
}

float Renderer::evaluateExtentForScaleFactor(float scaleFactor) {
    return (float) extent * scaleFactor;
}

float Renderer::evaluateCurrentExtent() {
    return evaluateExtentForScaleFactor(evaluateScaleFactorCurrentZ());
}

float Renderer::evaluateCameraDistance(float _scaleFactor, float zoomDiff) {
    float scale = evaluateScaleFactor(_scaleFactor, zoomDiff); // от 1 до cameraScaleOneUnitSphere * 2^19

    float resultDistance = 0;
    float zoomingDistance = cameraScaleOneUnitSphere * scale;
    resultDistance = zoomingDistance + planetRadius;

    LOGI("Camera distance %f", resultDistance);
    return resultDistance;
}

void Renderer::setupNoOpenGLMapState(float scaleFactor, AAssetManager *assetManager, JNIEnv *env) {
    loadAssets(assetManager);
    updateMapZoomScaleFactor(scaleFactor);
    updateCameraPosition();
    _savedLastScaleStateMapZ = realMapZTile();
    JavaVM* gJvm = nullptr;
    env->GetJavaVM(&gJvm);
    auto* getTileRequest = new GetTileRequest(cache, env);

    // network_tile_thread
    std::thread networkRootTileThread([this, gJvm, getTileRequest] { networkRootTileFunction(gJvm, getTileRequest); });
    networkRootTileThread.detach();
    networkTileThreads.push_back(&networkRootTileThread);

    networkTileThreadRunning.store(true);
    for(short i = 0; i < networkTilesThreads; i++) {
        std::thread networkTileThread([this, gJvm, getTileRequest] { networkTilesFunction(gJvm, getTileRequest); });
        networkTileThread.detach();
        networkTileThreads.push_back(&networkTileThread);
    }
}

void Renderer::loadAssets(AAssetManager *assetManager) {
    if(assetManager == nullptr)
        return;
    symbols->loadFont(assetManager);
}

void Renderer::updateFrustum() {
    // сколько нужно проскролить до нового зума
    float currentCameraDistance = evaluateCameraDistanceCurrentZ();
    float nextCameraDistance = evaluateCameraDistanceCurrentZ(1);
    float distanceDelta = currentCameraDistance - nextCameraDistance;

    float near = currentCameraDistance - planetRadius - distanceDelta;
    float far = currentCameraDistance + planetRadius * 10;
    projectionMatrix = EigenGL::createPerspectiveProjectionMatrix(
            fovy,
            (float) screenW / (float) screenH,
            near,
            far
    );
}

void Renderer::loadTextures(AAssetManager *assetManager) {
    defaultAvatarTextureId = loadTextureFromAsset("images/person.png", assetManager);
}

GLuint Renderer::loadTextureFromAsset(const char *assetName, AAssetManager *assetManager) {

    AAsset* asset = AAssetManager_open(assetManager, assetName, AASSET_MODE_UNKNOWN);
    off_t fileSize = AAsset_getLength(asset);
    auto* imageData = (unsigned char*) malloc(fileSize);
    AAsset_read(asset, imageData, fileSize);
    AAsset_close(asset);

    return loadTextureFromBytes(imageData, fileSize);
}

GLuint Renderer::loadTextureFromBytes(unsigned char* imageData, off_t fileSize) {
    int width, height, channels;
    unsigned char* image = stbi_load_from_memory(imageData, fileSize, &width, &height, &channels, STBI_rgb);

    unsigned int textureId;
    CommonUtils::printGlError();
    glGenTextures(1, &textureId);
    CommonUtils::printGlError();
    glBindTexture(GL_TEXTURE_2D, textureId);

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
    //free(imageData);
    delete[] image;
    return textureId;
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
}

void Renderer::updateMarkerGeo(std::string key, float latitude, float longitude) {
    auto markerIterator = userMarkers.find(key);
    if (markerIterator == userMarkers.end()) {
        return;
    }

    markerIterator->second.moveTo(latitude, longitude, flatRender);
}


void Renderer::removeMarker(std::string key) {
    auto markerIterator = userMarkers.find(key);
    if (markerIterator == userMarkers.end()) {
        return;
    }

    GLuint textureId = markerIterator->second.getTextureId();
    glDeleteTextures(1, &textureId);

    userMarkers.erase(markerIterator);
}

void Renderer::updateMarkerAvatar(std::string key, unsigned char* imageData, off_t fileSize) {
    auto markerIterator = userMarkers.find(key);
    if (markerIterator == userMarkers.end()) {
        return;
    }

    GLuint removeTextureId = markerIterator->second.getTextureId();
    glDeleteTextures(1, &removeTextureId);

    GLuint textureId = loadTextureFromBytes(imageData, fileSize);
    markerIterator->second.setTextureId(textureId);
}

void Renderer::handleMarker(std::string key, float latitude, float longitude, unsigned char *imageData, off_t fileSize) {
    auto markerIterator = userMarkers.find(key);
    if (markerIterator != userMarkers.end()) {
        GLuint removeTextureId = markerIterator->second.getTextureId();
        glDeleteTextures(1, &removeTextureId);
        GLuint textureId = loadTextureFromBytes(imageData, fileSize);
        markerIterator->second.setTextureId(textureId);
        markerIterator->second.moveTo(DEG2RAD(latitude), DEG2RAD(longitude), flatRender);
    } else {
        addMarker(key, latitude, longitude, imageData, fileSize);
    }
}

void Renderer::addMarker(std::string key, float latitude, float longitude, unsigned char* imageData, off_t fileSize) {
    auto markerIterator = userMarkers.find(key);
    if (markerIterator != userMarkers.end()) {
        return; // added already
    }

    GLuint textureId = loadTextureFromBytes(imageData, fileSize);
    auto marker = UserMarker(
            latitude,
            longitude,
            planetRadius,
            flatRender,
            evaluateInitMarkerSize(),
            textureId
    );

    userMarkers[key] = marker;
}

void Renderer::updateRenderTileProjection(short amountX, short amountY) {
    rendererTileProjectionMatrix = EigenGL::createOrthoMatrix(0, 4096.0 * amountX, -4096.0 * amountY, 0, 0.1, 100);
}

CornersCords Renderer::evaluateCorners(Eigen::Matrix4f& pvm, bool isFlat) {
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

    if (isFlat) {
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
    float A1 = firstPlane[0];
    float B1 = firstPlane[1];
    float C1 = firstPlane[2];
    float D1 = firstPlane[3];
    float d1 = A1 * planetRadius + D1;

    float A2 = firstPlane[0];
    float B2 = secondPlane[1];
    float C2 = secondPlane[2];
    float D2 = secondPlane[3];
    float d2 = A2 * planetRadius + D2;

    float det = B1 * C2 - B2 * C1;
    float detY = (-d1) * C2 - (-d2) * C1;
    float detZ = B1 * (-d2) - B2 * (-d1);

    float y0 = detY / det;
    float z0 = detZ / det;
    float x0 = planetRadius;

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
        planetGeometry.generateFlatData(tileX_start, tileX_end, tileY_start, tileY_end, zoom, tileGrain, planetRadius, flatTileSizeInit);
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

void Renderer::renderTiles(std::vector<TileCords> renderTiles, Eigen::Matrix4f pvmTexture, VisibleTilesResult visibleTilesResult) {
    std::shared_ptr<PlainShader> plainShader = shadersBucket->plainShader;

    int leftX = visibleTilesResult.visibleBlocks.tileX_start;
    int leftY = visibleTilesResult.visibleBlocks.tileY_start;
    int z = visibleTilesResult.visibleBlocks.zoom;
    int rightX = visibleTilesResult.visibleBlocks.tileX_end;

    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glUseProgram(plainShader->program);

    for(short geometryHeapIndex = Style::maxGeometryHeaps; geometryHeapIndex >= 0; --geometryHeapIndex) {
        for(auto& tileCords : renderTiles) {
            auto& visibleTile = tileCords;
            auto visibleTileKey = tileCords.getKey();

            Eigen::Matrix4f forTileMatrix;
            if (renderTileHash.find(visibleTileKey) == renderTileHash.end()) {
                short deltaTileZ = z - visibleTile.getTileZ();

                int visibleTileN = pow(2, visibleTile.getTileZ());
                int topLefTileN = pow(2, z);

                double visibleTileX = (double)visibleTile.getTileX() / visibleTileN;
                double visibleTileY = (double)visibleTile.getTileY() / visibleTileN;

                double topLeftTileX = (double)leftX / topLefTileN;
                double topLeftTileY = (double)leftY / topLefTileN;

                double deltaTileXPlanet = visibleTileX - topLeftTileX;
                double deltaTileYPlanet = visibleTileY - topLeftTileY;

                bool isVisibleTileOnRightSideOfEdge =
                        leftX > rightX &&
                        visibleTileX < topLeftTileX;
                if (isVisibleTileOnRightSideOfEdge) {
                    deltaTileXPlanet += 1;
                }

                // либо больше текущего зума, либо меньше текущего зума.
                // насколько он больше или меньше относительно текущего зума
                float scaleVisual2topLeft = pow(2, deltaTileZ);
                // размер относительно текущего видимого тайла
                // extent это всегда одна доля рендеринга тексутры всех тайлов
                // поэтому умножая на скейл получаем относительный размер
                float sizeOfVisualTile = extent * scaleVisual2topLeft;

                double tilesXDelta = deltaTileXPlanet * visibleTileN;
                double tilesYDelta = deltaTileYPlanet * visibleTileN;
                float deltaVisualX = tilesXDelta * sizeOfVisualTile;
                float deltaVisualY = -tilesYDelta * sizeOfVisualTile;

                Eigen::Affine3f modelTranslation(Eigen::Translation3f(deltaVisualX,deltaVisualY, 0));
                Eigen::Affine3f scaleMatrix(Eigen::Scaling(scaleVisual2topLeft, scaleVisual2topLeft, 0.0f));
                forTileMatrix = pvmTexture * modelTranslation.matrix() * scaleMatrix.matrix();

                renderTileHash[visibleTileKey] = RenderTileHash { forTileMatrix };
            } else {
                forTileMatrix = renderTileHash[visibleTileKey].forTileMatrix;
            }

            glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, forTileMatrix.data());
            Tile* tile = visibleTile.getDrawGeometry();

            // рисуем бекграунд
            if(geometryHeapIndex == Style::maxGeometryHeaps) {
                CSSColorParser::Color colorOfStyle = CSSColorParser::parse("rgb(241, 255, 230)");
                GLfloat red   = static_cast<GLfloat>(colorOfStyle.r) / 255;
                GLfloat green = static_cast<GLfloat>(colorOfStyle.g) / 255;
                GLfloat blue  = static_cast<GLfloat>(colorOfStyle.b) / 255;
                GLfloat alpha = static_cast<GLfloat>(colorOfStyle.a);
                const GLfloat color[] = { red, green, blue, alpha};
                glUniform4fv(plainShader->getColorLocation(), 1, color);

                float backPoints[] = {
                        0, 0,
                        (float) extent, 0,
                        (float) extent, -(float) extent,
                        0, 0,
                        0, - (float) extent,
                        (float) extent, -(float) extent,
                };

                glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT,
                                      GL_FALSE, 0, backPoints
                );
                glEnableVertexAttribArray(plainShader->getPosLocation());
                glDrawArrays(GL_TRIANGLES, 0, 6);
                continue;
            }

            // Рисуем основную карту
            if(geometryHeapIndex < Style::maxGeometryHeaps) {
                Geometry<float, unsigned int>& polygonsGeometry = tile->resultPolygons[geometryHeapIndex];
                Geometry<float, unsigned int>& linesGeometry = tile->resultLines[geometryHeapIndex];
                if(polygonsGeometry.isEmpty() && linesGeometry.isEmpty())
                    continue;

                float lineWidth = tile->style.getLineWidthOfHeap(geometryHeapIndex);
                glLineWidth(lineWidth);

                CSSColorParser::Color colorOfStyle = tile->style.getColorOfGeometryHeap(geometryHeapIndex);
                GLfloat red   = static_cast<GLfloat>(colorOfStyle.r) / 255;
                GLfloat green = static_cast<GLfloat>(colorOfStyle.g) / 255;
                GLfloat blue  = static_cast<GLfloat>(colorOfStyle.b) / 255;
                GLfloat alpha = static_cast<GLfloat>(colorOfStyle.a);
                const GLfloat color[] = { red, green, blue, alpha};
                glUniform4fv(plainShader->getColorLocation(), 1, color);


                if (!polygonsGeometry.isEmpty()) {
                    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT,
                                          GL_FALSE, 0, polygonsGeometry.points
                    );
                    glEnableVertexAttribArray(plainShader->getPosLocation());
                    glDrawElements(GL_TRIANGLES, polygonsGeometry.indicesCount, GL_UNSIGNED_INT, polygonsGeometry.indices);
                }

                if (!linesGeometry.isEmpty()) {
                    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT,
                                          GL_FALSE, 0, linesGeometry.points
                    );
                    glEnableVertexAttribArray(plainShader->getPosLocation());
                    glDrawElements(GL_LINES, linesGeometry.indicesCount, GL_UNSIGNED_INT, linesGeometry.indices);
                }

                continue;
            }
        }
    }
}

void Renderer::drawGlowing(Eigen::Matrix4f pvm) {
    glDisable(GL_DEPTH_TEST);
    auto planetGlowShader = shadersBucket->planetGlowShader;
    glUseProgram(planetGlowShader->program);
    Eigen::Matrix4f rotationY = EigenGL::createRotationMatrixY(-cameraLongitudeRad);
    Eigen::AngleAxisf rotationYBy90(-M_PI / 2, Eigen::Vector3f::UnitY());

    Eigen::Vector3f cameraVectorXZ(camWorldX, 0, camWorldZ);
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


// Test UI
Eigen::Matrix4f Renderer::evaluatePVM_UI() {
    Eigen::Vector3f cameraPosition = Eigen::Vector3f(0, 0, 1);
    Eigen::Vector3f target(0.0f, 0.0f, 0.0f);
    Eigen::Vector3f up(0.0f, 1.0f, 0.0f);
    Eigen::Matrix4f viewMatrixUI = EigenGL::createViewMatrix(cameraPosition, target, up);
    Eigen::Matrix4f projectionMatrixUI = EigenGL::createOrthoMatrix(0, screenW, 0, screenH, 0.1f, 1.0f);
    Eigen::Matrix4f pvmUI = projectionMatrixUI * viewMatrixUI;
    return pvmUI;
}

void Renderer::drawFPS_UI(Eigen::Matrix4f pvmUI) {
    glStencilMask(0X00);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);

    std::shared_ptr<SymbolShader> symbolShader = shadersBucket->symbolShader;
    CSSColorParser::Color color = CSSColorParser::parse("rgb(255, 255, 255)");
    symbols.get()->renderText("FPS: " + std::to_string((short)fps), 200, screenH - 100, pvmUI, 2, color);
}

void Renderer::drawTilesRenderTexture_UI(Eigen::Matrix4f pvmUI) {
    std::shared_ptr<TextureShader> textureShader = shadersBucket->textureShader;
    glUseProgram(textureShader->program);
    glStencilMask(0X00);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);

    float sizeUI = screenW / 4;
    float points[] = {
            0, sizeUI,
            0, 0,
            sizeUI, 0,
            sizeUI, sizeUI
    };
    GLfloat textureCords[] = {
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f
    };
    unsigned int indices[6] = {
            2, 3, 0,
            0, 1, 2
    };
    glUniformMatrix4fv(textureShader->getMatrixLocation(), 1, GL_FALSE, pvmUI.data());
    glBindTexture(GL_TEXTURE_2D, renderMapTexture);
    glVertexAttribPointer(textureShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, textureCords);
    glEnableVertexAttribArray(textureShader->getTextureCord());
    glVertexAttribPointer(textureShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, points);
    glEnableVertexAttribArray(textureShader->getPosLocation());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
}





