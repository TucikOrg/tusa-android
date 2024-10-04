//
// Created by Artem on 30.09.2024.
//

#include "MapRenderer2.h"

#include <cmath>


void MapRenderer2::renderFrame() {
    mapFpsCounter.newFrame();

    short tileZ = mapControls.getTilesZoom();
    int n = pow(2, tileZ);
    double EPSG3857LonNormInf = mapControls.getEPSG3857LongitudeNormInf();
    double EPSG3857LatNorm = mapControls.getEPSG3857LatitudeNorm();
    double EPSG3857CamLat = mapControls.getEPSG3857Latitude();
    double EPSG4326CamLat = mapControls.getEPSG4326Latitude();
    float distortion = mapControls.getVisPointDistortion();
    float transition = mapControls.getTransition();
    float invDistortion = 1.0 / distortion;

    float impact = mapControls.getCamDistDistortionImpact();
    float distancePortion = invDistortion * impact + (1.0 - impact);
    float distanceToMap = mapControls.getDistanceToMap(distancePortion);

    float planesDelta = distanceToMap / 1.1f;
    float nearPlane = distanceToMap - planesDelta;
    float farPlane = distanceToMap + planesDelta;
    float extent = 4096;


    float shiftPlaneY = -1.0f * (EPSG3857CamLat / M_PI) * (planeSize * 0.5f) * invDistortion;
    Eigen::Matrix4f scalePlane = EigenGL::createScaleMatrix(invDistortion, invDistortion, 1.0f);
    Eigen::Matrix4f translatePlane = EigenGL::createTranslationMatrix(0, shiftPlaneY, 0);
    Eigen::Matrix4f planeModelMatrix = translatePlane * scalePlane;
    Eigen::Matrix4f sphereModelMatrix = EigenGL::createRotationMatrixAxis(EPSG4326CamLat, Eigen::Vector3f {1.0, 0.0, 0.0});
    float scaledPlaneSize = planeSize * invDistortion;

    Eigen::Matrix4f projection = mapCamera.createPerspectiveProjection(nearPlane, farPlane);
    Eigen::Matrix4f view = mapCamera.createView(0, 0, distanceToMap, 0, 0, 0);
    Eigen::Matrix4f pv = projection * view;

    float textureTileSize = textureTileSizeUnit;
    auto plainShader = shadersBucket.plainShader.get();
    auto planet3Shader = shadersBucket.planet3Shader.get();

    float visYTilesDelta = 1.0;
    double visXTilesDelta = 1.0;
    if (n == 1) {
        visYTilesDelta = 0.5;
        visXTilesDelta = 0.5;
    }
    if (n == 4) {
        visXTilesDelta = 2.0;
        visYTilesDelta = 2.0;
    }
    float zoom = mapControls.getZoom();
    float maxTilesZoom = mapControls.getMaxTilesZoom();
    if (zoom > maxTilesZoom) {
        visYTilesDelta = 0.5;
        visXTilesDelta = 1.0;
        textureTileSize *= 2;
    }

    double camYNorm = (EPSG3857LatNorm - 1.0) / -2.0;

    double tileP = 1.0 / n;
    float camCenterYTile = camYNorm * n;
    float camYStart = std::fmax(0.0, camCenterYTile - visYTilesDelta);
    float camYEnd = std::fmin(n, camCenterYTile + visYTilesDelta);

    int visTileYStart = floor(camYStart);
    int visTileYEnd = ceil(camYEnd);
    int visTileYStartInv = n - visTileYEnd;
    int visTileYEndInv = n - visTileYStart;

    double camXNorm = (EPSG3857LonNormInf + 1.0) / 2.0;
    int yTilesAmount = visTileYEnd - visTileYStart;
    double camCenterXTile = camXNorm * n;
    double camXStart = camCenterXTile - visXTilesDelta;
    float camXEnd = camCenterXTile + visXTilesDelta;
    int visTileXStartInf = floor(camXStart);
    int visTileXEndInf = ceil(camXEnd);
    double xTilesAmount = abs(visTileXEndInf - visTileXStartInf);
    float scaleUTex = visXTilesDelta * 2.0 / xTilesAmount;
    double tileUSize = (1.0 / xTilesAmount);
    double tilesSwiped = (EPSG3857LonNormInf / (2.0 * tileP));
    double tilesUSwiped = tilesSwiped * tileUSize;
    double EPSGLonNormInfNegative = (EPSG3857LonNormInf < 0);
    float shiftUTex = fmod(tilesUSwiped, 1.0 / xTilesAmount) + EPSGLonNormInfNegative * tileUSize;

    double planetVStart = visTileYStartInv * tileP;
    double planetVEnd = visTileYEndInv * tileP;
    int segments = 40;
    double planetVDelta = (planetVEnd - planetVStart) / segments;
    double verticesShift = planeSize / 2.0;
    float squareSize = planeSize / segments;

    double planetUCenter = 0.5;
    double planetUVDelta = tileP * visXTilesDelta;
    double planetUStart = std::fmax(planetUCenter - planetUVDelta, 0.0);
    double planetUEnd = std::fmin(planetUCenter + planetUVDelta, 1.0);
    double planetUDelta = (planetUEnd - planetUStart) / segments;

    float topYVertex = -1;
    float leftXVertex = -1;
    float bottomYVertex = -1;
    float rightXVertex = -1;
    std::vector<float> planetEPSG3857;
    std::vector<float> planetTexUV;
    std::vector<float> planetVertices;
    for (int i = 0; i <= segments; i ++) {
        double planetV = planetVStart + i * planetVDelta;
        float y = planetV * planeSize - verticesShift;
        if (bottomYVertex == -1.0) bottomYVertex = y;
        topYVertex = y;

        for (int j = 0; j <= segments; j++) {
            double planetU = planetUStart + j * planetUDelta;
            float x = planetU * planeSize - verticesShift;
            if (leftXVertex == -1.0) leftXVertex = x;
            rightXVertex = x;

            planetVertices.push_back(x);
            planetVertices.push_back(y);
            planetEPSG3857.push_back((planetV * 2.0f - 1.0f) * M_PI);
            planetEPSG3857.push_back((planetU * 2.0f - 1.0f) * M_PI);

            planetTexUV.push_back(FLOAT(j) / segments);
            planetTexUV.push_back(FLOAT(i) / segments);
        }
    }

    std::vector<unsigned int> indices;
    for (int i = 0; i < segments; i++) {
        for (int j = 0; j <= segments; j++) {
            indices.push_back(i * (segments + 1) + j);
            indices.push_back((i + 1) * (segments + 1) + j);
        }
        if (i != segments - 1) {
            indices.push_back((i + 1) * (segments + 1) + segments);
            indices.push_back((i + 1) * (segments + 1));
        }
    }

    // определяем тайлы и ключ
    int existTiles = 0;
    std::vector<MapTile*> backgroundTiles = {};
    std::unordered_map<uint64_t, void*> backgroundTileKeys = {};
    std::unordered_map<uint64_t, MapTile*> tiles = {};
    for (int tileY = visTileYStart; tileY < visTileYEnd; tileY++) {
        for (int tileXInf = visTileXStartInf, xPos = 0; tileXInf < visTileXEndInf; tileXInf++, xPos++) {
            int tileX = normalizeXTile(tileXInf, n);
            auto tile = mapTileGetter->getOrRequest(tileX, tileY, tileZ);
            if (tile->isEmpty()) {
                // Нужна замена на фоновый тайл
                auto replacement = mapTileGetter->findExistParent(tileX, tileY, tileZ);
                if (!replacement->isEmpty()) {
                    auto addKey = MapTile::makeKey(replacement->getX(), replacement->getY(), replacement->getZ());
                    if (backgroundTileKeys.find(addKey) == backgroundTileKeys.end()) {
                        backgroundTiles.push_back(replacement);
                        backgroundTileKeys.insert({addKey, nullptr});
                    }
                }
            }
            existTiles += !tile->isEmpty();
            tiles.insert({MapTile::makeKey(tileX, tileY, tileZ), tile});
        }
    }

    std::string newTextureKey =
            std::to_string(visTileYStart) +
            std::to_string(visTileYEnd) +
            std::to_string(visTileXStartInf) +
            std::to_string(visTileXEndInf) +
            std::to_string(tileZ) +
            std::to_string(backgroundTiles.size()) +
            std::to_string(existTiles);


    //////////////////////////////////
    ////                            //
    ////       Render texture       //
    ////                            //
    //////////////////////////////////
    if (textureKey != newTextureKey) {
        textureKey = newTextureKey;
        float topY = visTileYStart;
        float leftX = normalizeXTile(visTileXStartInf, n);
        float textureWidth = textureTileSize * xTilesAmount;
        float textureHeight = textureTileSize * yTilesAmount;
        if (prTex2dHeight != textureHeight || prTex2dWidth != textureWidth) {
            glBindTexture(GL_TEXTURE_2D, mapTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
            prTex2dHeight = textureHeight;
            prTex2dWidth = textureWidth;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, mapTextureFramebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mapTexture, 0);
        glViewport(0, 0, textureWidth, textureHeight);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_SCISSOR_TEST);
        Eigen::Matrix4f projectionTexture = mapCamera.createOrthoProjection(0, xTilesAmount * extent, yTilesAmount * -extent, 0, 0.1, 1);
        Eigen::Matrix4f viewTexture = mapCamera.createView(0, 0, 1, 0, 0, 0);
        Eigen::Matrix4f pvTexture = projectionTexture * viewTexture;

        // рисуем фоновые тайлы
        for (int loop = -1; loop <= 1; loop++) {
            for (auto backgroundTile : backgroundTiles) {
                float deltaZ = tileZ - backgroundTile->getZ();
                float scale = pow(2.0, deltaZ);

                float translateXIndex = loop * n + (backgroundTile->getX() * scale - leftX);
                float translateYIndex = (backgroundTile->getY() * scale - topY);
                float translateY = translateYIndex * -extent;
                float translateX = translateXIndex * extent;
                float scissorX = translateXIndex * textureTileSize;
                float scissorY = yTilesAmount * textureTileSize - (translateYIndex + 1 * scale) * textureTileSize;
                float backgroundTileTexSize = textureTileSize * scale;
                if (scissorX + backgroundTileTexSize < 0 || scissorX > textureWidth) {
                    continue;
                }
                glScissor(scissorX, scissorY, backgroundTileTexSize, backgroundTileTexSize);
                auto scaleM = EigenGL::createScaleMatrix(scale, scale, 1.0);
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

        // рисуем актуальные тайлы
        for (int tileY = visTileYStart; tileY < visTileYEnd; tileY++) {
            for (int tileXInf = visTileXStartInf, xPos = 0; tileXInf < visTileXEndInf; tileXInf++, xPos++) {
                int tileX = normalizeXTile(tileXInf, n);
                auto tile = tiles[MapTile::makeKey(tileX, tileY, tileZ)];
                if (tile->isEmpty()) {
                    continue;
                }
                float translateXIndex = xPos;
                float translateYIndex = tileY - visTileYStart;
                float translateX = translateXIndex * extent;
                float translateY = translateYIndex * -extent;
                float scissorX = translateXIndex * textureTileSize;
                float scissorY = yTilesAmount * textureTileSize - (translateYIndex + 1) * textureTileSize;
                glScissor(scissorX, scissorY, textureTileSize, textureTileSize);
                auto tileModelMatrix = EigenGL::createTranslationMatrix(translateX, translateY, 0);
                Eigen::Matrix4f tileMatrix = pvTexture * tileModelMatrix;
                mapTileRender.renderTile(shadersBucket, tile, mapCamera, tileMatrix);
            }
        }
        glDisable(GL_SCISSOR_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        mapCamera.glViewportDeviceSize();
    }


    bool forwardRenderingToWorld = zoom >= 6;

    //////////////////////////////////
    ////                            //
    ////       Render planet        //
    ////                            //
    //////////////////////////////////
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (forwardRenderingToWorld) {
        float shiftXTileP = fmod(tilesSwiped, 1.0) + EPSGLonNormInfNegative;
        Eigen::Vector4f topLeftWorld4f = planeModelMatrix * Eigen::Vector4f(leftXVertex, topYVertex, 0, 1.0);
        Eigen::Vector4f bottomRightWorld4f = planeModelMatrix * Eigen::Vector4f(rightXVertex, bottomYVertex, 0, 1.0);
        Eigen::Vector3f topLeftWorld = topLeftWorld4f.head(3) / topLeftWorld4f.w();
        Eigen::Vector3f bottomRightWorld = bottomRightWorld4f.head(3) / bottomRightWorld4f.w();
        float worldTileSizeX = abs(bottomRightWorld.x() - topLeftWorld.x()) / (2.0 * visXTilesDelta);
        float worldTileSizeY = abs(bottomRightWorld.y() - topLeftWorld.y()) / yTilesAmount;
        float scaleTileX = worldTileSizeX / extent;
        float scaleTileY = worldTileSizeY / extent;

        // рисуем фоновые тайлы
        for (int loop = -1; loop <= 1; loop++) {
            for (auto backgroundTile : backgroundTiles) {
                float deltaZ = tileZ - backgroundTile->getZ();
                float scale = pow(2.0, deltaZ);

                float translateXIndex = loop * n + (backgroundTile->getX() * scale - leftX);
                float translateYIndex = (backgroundTile->getY() * scale - topY);
                float translateY = translateYIndex * -extent;
                float translateX = translateXIndex * extent;
                auto scaleM = EigenGL::createScaleMatrix(scale, scale, 1.0);
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

        std::vector<TileAndMatrix> actualTiles = {};
        // рисуем актуальные тайлы
        for (int tileY = visTileYStart; tileY < visTileYEnd; tileY++) {
            for (int tileXInf = visTileXStartInf, xPos = 0; tileXInf < visTileXEndInf; tileXInf++, xPos++) {
                int tileX = normalizeXTile(tileXInf, n);
                auto tile = tiles[MapTile::makeKey(tileX, tileY, tileZ)];
                if (tile->isEmpty()) {
                    continue;
                }
                float translateXIndex = xPos;
                float translateYIndex = tileY - visTileYStart;
                float translateX = (translateXIndex - shiftXTileP) * worldTileSizeX + topLeftWorld.x();
                float translateY = translateYIndex * -worldTileSizeY + topLeftWorld.y();
                auto translateMatrix = EigenGL::createTranslationMatrix(translateX, translateY, 0);
                Eigen::Matrix4f scaleMatrix = EigenGL::createScaleMatrix(scaleTileX, scaleTileY, 1.0);
                Eigen::Matrix4f tileMatrix = pv * translateMatrix * scaleMatrix;
                actualTiles.push_back({tile, tileMatrix});
            }
        }
        mapTileRender.renderTilesByLayers(shadersBucket, actualTiles);

    } else {
        glEnable(GL_DEPTH_TEST);
        glUseProgram(planet3Shader->program);
        glUniformMatrix4fv(planet3Shader->getMatrixLocation(), 1, GL_FALSE, pv.data());
        glUniformMatrix4fv(planet3Shader->getPlaneMatrixLocation(), 1, GL_FALSE, planeModelMatrix.data());
        glUniformMatrix4fv(planet3Shader->getSphereMatrixLocation(), 1, GL_FALSE, sphereModelMatrix.data());
        glVertexAttribPointer(planet3Shader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, planetVertices.data());
        glEnableVertexAttribArray(planet3Shader->getPosLocation());
        glVertexAttribPointer(planet3Shader->getPlanetEPSG3857Location(), 2, GL_FLOAT, GL_FALSE, 0, planetEPSG3857.data());
        glEnableVertexAttribArray(planet3Shader->getPlanetEPSG3857Location());
        glVertexAttribPointer(planet3Shader->getTextureUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, planetTexUV.data());
        glEnableVertexAttribArray(planet3Shader->getTextureUVLocation());
        glBindTexture(GL_TEXTURE_2D, mapTexture);
        glUniform1i(planet3Shader->getTextureLocation(), 0);
        glUniform1f(planet3Shader->getPlaneSizeLocation(), planeSize);
        glUniform1f(planet3Shader->getTransitionLocation(), transition);
        glUniform2f(planet3Shader->getCameraEPSG3857Location(), EPSG3857CamLat, 0.0f);
        glUniform2f(planet3Shader->getUVOffsetLocation(), shiftUTex, 0.0f);
        glUniform2f(planet3Shader->getUVScaleLocation(), scaleUTex, 1.0f);
        glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, indices.data());
        glDisable(GL_DEPTH_TEST);
    }


    // draw points
    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, planetVertices.data());
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glUniform4f(plainShader->getColorLocation(), 1.0, 0.0, 0.0, 1.0);
    glUniform1f(plainShader->getPointSizeLocation(), 8.0f);
    //glDrawArrays(GL_POINTS, 0, planetVertices.size() / 2);

    // draw center point
    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
    float centerPoint[] = {0, 0};
    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, centerPoint);
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glUniform4f(plainShader->getColorLocation(), 1.0, 0.0, 0.0, 1.0);
    glUniform1f(plainShader->getPointSizeLocation(), 10.0f);
    glDrawArrays(GL_POINTS, 0, 1);

    // draw texture test
    auto textureShader = shadersBucket.textureShader;
    float ratio = mapCamera.getRatio();
    float testTextureWindowSize = 0.1;
    float width = testTextureWindowSize * xTilesAmount;
    float height = testTextureWindowSize * yTilesAmount;
    std::vector<float> testVertices = {
            -ratio, -1.0f,
            -ratio + width, -1.0f,
            -ratio + width, -1.0f + height,
            -ratio, -1.0f + height
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
    glUniform4f(plainShader->getColorLocation(), 1.0, 0.0, 0.0, 1.0);
    glVertexAttribPointer(textureShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, uvTest.data());
    glEnableVertexAttribArray(textureShader->getTextureCord());
    glBindTexture(GL_TEXTURE_2D, mapTexture);
    glUniform1f(textureShader->getTileTextureLocation0(), 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    mapTest.drawFPS(shadersBucket, mapSymbols, mapCamera, mapFpsCounter.getFps());
}

void MapRenderer2::init(AAssetManager *assetManager, JNIEnv *env, jobject &request_tile) {
    mapTileGetter = new MapTileGetter(env, request_tile, mapTileRender.getStyle());
    mapTileGetter->getOrRequest(0, 0, 0);
    mapSymbols.loadFont(assetManager);
}

void MapRenderer2::onSurfaceChanged(int screenW, int screenH) {
    mapCamera = MapCamera(screenW, screenH, 60.0f);
    mapTest.init(mapCamera);
}

void MapRenderer2::onSurfaceCreated(AAssetManager *assetManager) {
    shadersBucket.compileAllShaders(assetManager);
    mapSymbols.createFontTextures();
    mapTileRender.initTilesTexture();

    glGenTextures(1, &mapTexture);
    glBindTexture(GL_TEXTURE_2D, mapTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &mapTextureFramebuffer);
}

void MapRenderer2::drag(float dx, float dy) {
    mapControls.drag(dx, dy);
}

void MapRenderer2::scale(float scaleFactor) {
    mapControls.scale(scaleFactor);
}

void MapRenderer2::doubleTap() {
    mapControls.doubleTap();
}

MapRenderer2::MapRenderer2() {
    float moscowLat = DEG2RAD(55.7558);
    float moscowLon = DEG2RAD(37.6176);
    mapControls.setCamPos(moscowLat, moscowLon);
    mapControls.setZoom(14);
}


