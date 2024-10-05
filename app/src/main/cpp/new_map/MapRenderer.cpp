//
// Created by Artem on 16.09.2024.
//

#include "MapRenderer.h"
#include "MapRenderer2.h"

#include <GLES2/gl2.h>


void MapRenderer::renderFrame() {
    render2DMap();
}

void MapRenderer::render2DMap() {
    float deltaTime               = mapFpsCounter.newFrame();
    float timeElapsed             = mapFpsCounter.getTimeElapsed();
    float tileTextureSize         = 1024;
    float tileSize                = 1.0f;
    int zTile                     = mapControls.getTilesZoom();
    int n = pow(2.0, zTile);
    float distanceToMap           = mapControls.getDistanceToMap();
    float projectionNearFarDelta  = distanceToMap / 1.1;
    float planeZ                  = radius;
    float cameraDistance          = distanceToMap + radius;
    float cameraY                 = mapControls.getCameraY();
    float nearPlane               = distanceToMap - projectionNearFarDelta;
    float farPlane                = distanceToMap + projectionNearFarDelta;
    float aspectRatio             = mapCamera.getRatio();
    int lod                       = 20;
    float cameraLatitudeRad       = mapControls.getCameraSphereLatitude(planeWidth);
    float distortionV = distortion(cameraLatitudeRad);
    LOGI("distortion: %f", distortionV);

    float visibleWindowX = 2;
    if (zTile == 2) {
        visibleWindowX = 4;
    }
    mapControls.checkScale(visibleWindowX);
    float shiftX = mapControls.getShiftX();
    if (n == 1) {
        visibleWindowX = 1;
        tileTextureSize *= 2;
    }


    Eigen::Matrix4f viewToMapPlane = mapCamera.createView(0, cameraY, cameraDistance, 0, cameraY, 0);
    Eigen::Matrix4f projectionToMapPlane = mapCamera.createPerspectiveProjection(distanceToMap - 1.0, distanceToMap);
    Eigen::Matrix4f pvToMapPlane = projectionToMapPlane * viewToMapPlane;

    auto planet2Shader = shadersBucket.planet2Shader;
    float halfWidth         = planeWidth / 2.0;
    float uTileDelta        = 1.0 / n;
    float uTileLodDelta     = uTileDelta / lod;
    float tileSizePlanet    = planeWidth / n;

    float lodDelta          = tileSizePlanet / lod;
    float halfPlane         = planeWidth / 2.0;
    Eigen::Matrix4f pvInverse = pvToMapPlane.inverse();
    float w = 1.0f;
    Eigen::Vector4f ndcTopMiddle(0.0f, 1.0f, w, 1.0f);
    Eigen::Vector4f topMiddlePoint = pvInverse * ndcTopMiddle;
    Eigen::Vector4f ndcBottomMiddle(0.0f, -1.0f, w, 1.0f);
    Eigen::Vector4f bottomMiddlePoint = pvInverse * ndcBottomMiddle;
    topMiddlePoint /= topMiddlePoint.w();
    bottomMiddlePoint /= bottomMiddlePoint.w();

    float yAdditional = 0.0;
    if (zTile == 2 || zTile == 1) {
        yAdditional = 1.0;
    }
    float yTop = (topMiddlePoint.y() - halfPlane) / -planeWidth - yAdditional;
    float yBottom = (bottomMiddlePoint.y() - halfPlane) / -planeWidth + yAdditional;
    yTop = std::fmax(0.0, std::fmin(1.0, yTop));
    yBottom = std::fmax(0.0, std::fmin(1.0, yBottom));
    float yTileTop = floor(yTop * n);
    float yTileBottom = ceil(yBottom * n) - 1;
    int yTiles = yTileBottom - yTileTop + 1;

    float mapSize = n * tileSize;
    float visibleWindowY = yTiles;
    float xWorldWindowSize = tileSize * visibleWindowX;
    float yWorldWindowSize = tileSize * visibleWindowY;
    float camX = shiftX;
    float camY = - yWorldWindowSize;



    //////////////////////////////////
    ////                            //
    ////   Render 2d map texture    //
    ////                            //
    //////////////////////////////////
    std::vector<std::array<int, 2>> xTiles;

    float lefVisibleX = shiftX;
    float rightVisibleX = shiftX + xWorldWindowSize;
    for (int xTileWorld = floor(lefVisibleX / tileSize); xTileWorld <= floor(rightVisibleX / tileSize); xTileWorld += 1) {
        int xTile = fmod(xTileWorld, n);
        if (xTile < 0) {
            xTile += n;
        }
        xTiles.push_back({xTile, xTileWorld});
    }
    int xTilesSize = xTiles.size();

    std::unordered_map<uint64_t, MapTile*> tiles;
    // Определяем ключ текстуры
    int existTiles = 0;
    for (int yTile = yTileTop; yTile <= yTileBottom; yTile++) {
        for (auto xTileTuple: xTiles) {
            int xTile = xTileTuple[0];
            auto tile = mapTileGetter->getOrRequest(xTile, yTile, zTile);
            existTiles += !tile->isEmpty();
            tiles.insert({MapTile::makeKey(xTile, yTile, zTile), tile});
        }
    }
    std::string newTextureKey =
            std::to_string(xTiles[0][0]) +
            std::to_string(xTiles[xTilesSize - 1][0]) +
            std::to_string(yTileTop) +
            std::to_string(yTileBottom) +
            std::to_string(zTile) +
            std::to_string(existTiles);

    float extent = 4096;
    int textureWidth = xTilesSize * tileTextureSize;
    int textureHeight = yTiles * tileTextureSize;
    if (textureKey != newTextureKey) {
        textureKey = newTextureKey;

        // определяем фоновые тайлы
        std::vector<MapTile*> backgroundTiles;
        std::unordered_map<uint64_t, void*> backgroundTileKeys;
        for (int yTile = yTileTop; yTile <= yTileBottom; yTile++) {
            for (auto xTileTuple: xTiles) {
                int xTile = xTileTuple[0];
                auto tile = tiles[MapTile::makeKey(xTile, yTile, zTile)];
                if (tile->isEmpty()) {
                    // Нужна замена на фоновый тайл
                    auto replacement = mapTileGetter->findExistParent(xTile, yTile, zTile);
                    if (!replacement->isEmpty()) {
                        auto addKey = MapTile::makeKey(replacement->getX(), replacement->getY(), replacement->getZ());
                        if (backgroundTileKeys.find(addKey) != backgroundTileKeys.end()) {
                            continue;
                        }

                        backgroundTiles.push_back(replacement);
                        backgroundTileKeys.insert({addKey, nullptr});
                    }
                    continue;
                }
            }
        }

        if (prTex2dHeight != textureHeight || prTex2dWidth != textureWidth) {
            glBindTexture(GL_TEXTURE_2D, mapTexture2D);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
            prTex2dHeight = textureHeight;
            prTex2dWidth = textureWidth;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, mapFrameBuffer2D);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mapTexture2D, 0);
        glViewport(0, 0, textureWidth, textureHeight);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_SCISSOR_TEST);

        auto projectionTexture = mapCamera.createOrthoProjection(0, xTilesSize * extent, yTiles * -extent, 0, 0.1, 1);
        auto viewTexture = mapCamera.createView(0, 0, 1, 0, 0, 0);
        Eigen::Matrix4f pvTexture = projectionTexture * viewTexture;
        int xTranslateLeft = xTiles[0][1];
        auto leftXTile = xTiles[0];

        // рисуем фоновые тайлы
        auto plainShader = shadersBucket.plainShader;
        for (int loop = -1; loop <= 1; loop++) {
            for (auto backgroundTile : backgroundTiles) {
                float deltaZ = zTile - backgroundTile->getZ();
                float scale = pow(2.0, deltaZ);

                float translateXIndex = (backgroundTile->getX() * scale - leftXTile[0]) + (loop * n);
                float translateYIndex = (backgroundTile->getY() * scale - yTileTop);
                float translateX = translateXIndex * extent;
                float translateY = translateYIndex * -extent;
                float scissorX = translateXIndex * tileTextureSize;
                float scissorY = yTiles * tileTextureSize - (translateYIndex + 1 * scale) * tileTextureSize;
                float backgroundTileTexSize = tileTextureSize * scale;
                if (scissorX + backgroundTileTexSize < 0 || scissorX > textureWidth) {
                    continue;
                }
                glScissor(scissorX, scissorY, backgroundTileTexSize, backgroundTileTexSize);
                auto scaleM = EigenGL::createScaleMatrix(scale, scale, 1.0f);
                auto translate = EigenGL::createTranslationMatrix(translateX, translateY, 0);
                Eigen::Matrix4f tileMatrix = pvTexture * translate * scaleM;
                mapTileRender.renderTile(
                        shadersBucket,
                        backgroundTile,
                        mapCamera,
                        tileMatrix
                );
            }
        }

        // рисуем видимые тайлы
        for (int yTile = yTileTop; yTile <= yTileBottom; yTile++) {
            for (auto xTileTuple : xTiles) {
                int xTile = xTileTuple[0];
                int xTranslate = xTileTuple[1];
                auto tile = tiles[MapTile::makeKey(xTile, yTile, zTile)];
                if (tile->isEmpty()) {
                    continue;
                }

                float translateXIndex = (xTranslate - xTranslateLeft);
                float translateYIndex = (yTile - yTileTop);
                float translateX = translateXIndex * extent;
                float translateY = translateYIndex * -extent;
                auto translate = EigenGL::createTranslationMatrix(translateX, translateY, 0);
                Eigen::Matrix4f tileMatrix = pvTexture * translate;
                float scissorX = translateXIndex * tileTextureSize;
                float scissorY = yTiles * tileTextureSize - (translateYIndex + 1) * tileTextureSize;
                glScissor(scissorX, scissorY, tileTextureSize, tileTextureSize);
                mapTileRender.renderTile(
                        shadersBucket,
                        tile,
                        mapCamera,
                        tileMatrix
                );
            }
        }

        glDisable(GL_SCISSOR_TEST);
    }


    //////////////////////////////////
    ////                            //
    ////       Render 2d map        //
    ////                            //
    //////////////////////////////////
    std::vector<float> uv;
    std::vector<float> vertices;
    auto geometryXShift = xTiles[0][1] * tileSize;
    for (int i = 0; i <= yTiles; i++) {
        for (int j = 0; j <= xTilesSize; j++) {
            vertices.push_back(j * tileSize + geometryXShift); // x
            vertices.push_back(-i * tileSize); // y
            float u = j / FLOAT(xTilesSize);
            float v = 1.0f - (i) / FLOAT(yTiles);
            uv.push_back(u);
            uv.push_back(v);
        }
    }

    std::vector<unsigned int> indices;
    for (int i = 0; i < yTiles; i++) {
        for (int j = 0; j <= xTilesSize; j++) {
            // Добавляем две вершины для полосы треугольников
            indices.push_back(i * (xTilesSize + 1) + j);        // Верхняя вершина
            indices.push_back((i + 1) * (xTilesSize + 1) + j);  // Нижняя вершина
        }
        // Если не последняя строка, добавляем degenerate triangles
        // (дублируем последнюю и первую вершины для предотвращения перехода)
        if (i != yTiles - 1) {
            indices.push_back((i + 1) * (xTilesSize + 1) + xTilesSize);  // Повторяем последнюю вершину
            indices.push_back((i + 1) * (xTilesSize + 1));      // Переходим к началу новой строки
        }
    }

    auto textureShader = shadersBucket.textureShader;
    auto plainShader = shadersBucket.plainShader;
    if (prTexPlanetHeight != textureHeight || prTexPlanetWidth != textureWidth) {
        glBindTexture(GL_TEXTURE_2D, mapTextureForPlanet);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        prTexPlanetHeight = textureHeight;
        prTexPlanetWidth = textureWidth;
    }

    Eigen::Matrix4f projection = mapCamera.createOrthoProjection(0.0, xWorldWindowSize, 0.0, yWorldWindowSize, 0.1, 1.0);
    Eigen::Matrix4f view = mapCamera.createView(camX, camY, 1.0f, camX, camY, 0.0f);
    Eigen::Matrix4f pv = projection * view;

    glBindFramebuffer(GL_FRAMEBUFFER, mapFrameBufferForPlanet);
    glClearColor(0.3, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, textureWidth, textureHeight);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mapTextureForPlanet, 0);
    glUseProgram(textureShader->program);
    glBindTexture(GL_TEXTURE_2D, mapTexture2D);
    glUniform1f(textureShader->getTileTextureLocation0(), 0);
    glVertexAttribPointer(textureShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glEnableVertexAttribArray(textureShader->getPosLocation());
    glVertexAttribPointer(textureShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, uv.data());
    glEnableVertexAttribArray(textureShader->getTextureCord());
    glUniformMatrix4fv(textureShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, indices.data());

    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
    glUniform4f(plainShader->getColorLocation(), 1.0, 0.0, 0.0, 1.0);
    glUniform1f(plainShader->getPointSizeLocation(), 15.0);
    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glDrawArrays(GL_POINTS, 0, vertices.size() / 2);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mapCamera.glViewportDeviceSize();
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //////////////////////////////////
    ////                            //
    ////       Render planet        //
    ////                            //
    //////////////////////////////////
    int yPointsAmount   = yTiles * lod + 1;
    int xPointsAmount   = visibleWindowX * lod + 1;
    int xPointLastIndex = xPointsAmount - 1;
    int yPointLastIndex = yPointsAmount - 1;
    float uDeltaXTilesSize    = visibleWindowX * uTileDelta;
    float planetStartU        = 0.5 - uDeltaXTilesSize / 2.0;
    int xiEnd                 = visibleWindowX - 1;
    float xShift              = -visibleWindowX * tileSizePlanet / 2.0;
    std::vector<float> uNowTest;
    std::vector<float> vNowTest;
    std::vector<float> planetVTest;
    std::vector<float> planetUTest;
    std::vector<float> verticesPlanet;
    std::vector<float> planetUV;
    std::vector<float> planetUVTexture;
    std::vector<unsigned int> planetIndices;
    for (int xi = 0; xi <= xiEnd; xi++) {
        int toXLod = lod + (xi == xiEnd);
        for (int xLod = 0; xLod < toXLod; xLod++) {
            float x = tileSizePlanet * xi + xLod * lodDelta + xShift;
            float uNow = (FLOAT(xi * lod) + xLod) / ((xiEnd + 1) * lod);
            float planetU = planetStartU + uTileLodDelta * (xLod + xi * lod);
            planetUTest.push_back(planetU);
            uNowTest.push_back(uNow);
            for (int yi = yTileTop; yi <= yTileBottom; yi++) {
                int toYLod = lod + (yi == yTileBottom);
                float yIndex = yi - yTileTop;
                for (int yLod = 0; yLod < toYLod; yLod++) {
                    float y = halfWidth - yi * tileSizePlanet - yLod * lodDelta;
                    verticesPlanet.push_back(x);
                    verticesPlanet.push_back(y);
                    verticesPlanet.push_back(planeZ);
                    float vNow = 1.0 - (yIndex * lod + yLod) / (yTiles * lod);
                    float planetV = (yi + FLOAT(yLod) / lod) / n;
                    if (xi == 0 && xLod == 0) {
                        vNowTest.push_back(vNow);
                        planetVTest.push_back(planetV);
                    }
                    planetUVTexture.push_back(uNow);
                    planetUVTexture.push_back(vNow);
                    planetUV.push_back(planetU);
                    planetUV.push_back(planetV);
                }
            }
        }
    }

    int xIndicesEnd = visibleWindowX * lod;
    for (int x = 0; x < xIndicesEnd; x++) {
        for (int y = 0; y < yPointsAmount - 1; y++) {
            int i = x * yPointsAmount + y;
            planetIndices.push_back(i);
            planetIndices.push_back(i + 1);
            planetIndices.push_back(i + yPointsAmount);
            planetIndices.push_back(i + 1);
            planetIndices.push_back(i + yPointsAmount);
            planetIndices.push_back(i + yPointsAmount + 1);
        }
    }
    Eigen::Matrix4f projectionPlanet = mapCamera.createPerspectiveProjection(nearPlane, farPlane);

    glEnable(GL_DEPTH_TEST);
    glUseProgram(planet2Shader->program);
    glUniformMatrix4fv(planet2Shader->getProjectionMatrixLocation(), 1, GL_FALSE, projectionPlanet.data());
    glVertexAttribPointer(planet2Shader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, verticesPlanet.data());
    glEnableVertexAttribArray(planet2Shader->getPosLocation());
    glVertexAttribPointer(planet2Shader->getPlanetUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, planetUV.data());
    glEnableVertexAttribArray(planet2Shader->getPlanetUVLocation());
    glVertexAttribPointer(planet2Shader->getTilesUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, planetUVTexture.data());
    glEnableVertexAttribArray(planet2Shader->getTilesUVLocation());
    glBindTexture(GL_TEXTURE_2D, mapTextureForPlanet);
    glUniform1i(planet2Shader->getTileTextureLocation(), 0);
    glUniform1f(planet2Shader->getTransitionLocation(), mapControls.getAnimatedTransition(timeElapsed * 2.0));
    glUniform1f(planet2Shader->getUsePolesZoomingLocation(), 1.0f);
    glUniform1f(planet2Shader->getCamAngleLocation(), cameraLatitudeRad);
    glUniform1f(planet2Shader->getCamDistanceLocation(), cameraDistance);
    glUniform1f(planet2Shader->getRadiusLocation(), radius);
    glUniform1f(planet2Shader->getPlaneCamYLocation(), cameraY);
    glDrawElements(GL_TRIANGLES, planetIndices.size(), GL_UNSIGNED_INT, planetIndices.data());
    glDisable(GL_DEPTH_TEST);

    Eigen::Matrix4f pvPlanet = projectionPlanet * viewToMapPlane;
    mapTest.drawFPS(shadersBucket, mapSymbols, mapCamera, mapFpsCounter.getFps());
    mapTest.drawPoints3D(shadersBucket, verticesPlanet, 5.0f, pvPlanet);
    mapTest.drawTilesTextureTest(shadersBucket, mapCamera, mapTextureForPlanet, visibleWindowX, visibleWindowY);

    auto error = CommonUtils::getGLErrorString();
}

void MapRenderer::onSurfaceChanged(int screenW, int screenH) {
    mapCamera = MapCamera(screenW, screenH, 60.0f);
    mapTest.init(mapCamera);
}

void MapRenderer::onSurfaceCreated(AAssetManager* assetManager) {
    shadersBucket.compileAllShaders(assetManager);
    mapSymbols.createFontTextures();
    mapTileRender.initTilesTexture();

    glGenFramebuffers(1, &mapFrameBuffer2D);
    glGenFramebuffers(1, &mapFrameBufferForPlanet);

    glGenTextures(1, &mapTexture2D);
    glBindTexture(GL_TEXTURE_2D, mapTexture2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &mapTextureForPlanet);
    glBindTexture(GL_TEXTURE_2D, mapTextureForPlanet);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

void MapRenderer::init(AAssetManager *assetManager, JNIEnv *env, jobject& request_tile) {
    mapTileGetter = new MapTileGetter(env, request_tile, mapTileRender.getStyle());
    mapTileGetter->getOrRequest(0, 0, 0);
    mapSymbols.loadFont(assetManager);

}

void MapRenderer::drag(float dx, float dy) {
    mapControls.drag(dx, dy);
}

void MapRenderer::scale(float scaleFactor) {
    mapControls.scale(scaleFactor);
}

void MapRenderer::doubleTap() {
    mapControls.doubleTap();
}

MapRenderer::~MapRenderer() {
    delete mapTileGetter;
}

MapRenderer::MapRenderer() {
    mapControls.setCamYLimit(planeWidth / 2.0);
    mapControls.initStartZoom(0);
}


