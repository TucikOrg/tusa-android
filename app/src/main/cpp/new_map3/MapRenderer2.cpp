//
// Created by Artem on 30.09.2024.
//

#include "MapRenderer2.h"

#include <cmath>

void MapRenderer2::renderFrame() {
    mapFpsCounter.newFrame();

    short tileZ = mapControls.getTilesZoom();
    float zoom = mapControls.getZoom();
    int n = pow(2, tileZ);
    double EPSG3857LonNormInf = mapControls.getEPSG3857LongitudeNormInf();
    double EPSG3857CamLatNorm = mapControls.getEPSG3857LatitudeNorm();
    double EPSG3857CamLat = mapControls.getEPSG3857Latitude();
    double EPSG4326CamLat = mapControls.getEPSG4326Latitude();
    double distortion = mapControls.getVisPointDistortion();
    float transition = mapControls.getTransition();
    double invDistortion = 1.0 / distortion;

    float impact = mapControls.getCamDistDistortionImpact();
    float distancePortion = invDistortion * impact + (1.0 - impact);
    float distanceToMap = mapControls.getDistanceToMap(distancePortion);

    float planesDelta = distanceToMap / 1.1f;
    float nearPlane = distanceToMap - planesDelta;
    float farPlane = distanceToMap + planesDelta;
    float extent = 4096;

    double shiftPlaneY = -1.0 * EPSG3857CamLatNorm * (planeSize * 0.5 / distortion);
    Eigen::Matrix4d scalePlane = EigenGL::createScaleMatrix(invDistortion, invDistortion, 1.0);
    Eigen::Matrix4d translatePlane = EigenGL::createTranslationMatrix(0.0, shiftPlaneY, 0.0);
    Eigen::Matrix4d planeModelMatrix = translatePlane * scalePlane;
    Eigen::Matrix4f sphereModelMatrix = EigenGL::createRotationMatrixAxis(FLOAT(EPSG4326CamLat), Eigen::Vector3f {1.0, 0.0, 0.0});

    Eigen::Matrix4d projection = mapCamera.createPerspectiveProjectionD(nearPlane, farPlane);
    Eigen::Matrix4d view = mapCamera.createViewD(0, 0, distanceToMap, 0, 0, 0);
    Eigen::Matrix4d pv = projection * view;

    Eigen::Matrix4f pvFloat = pv.cast<float>();
    Eigen::Matrix4f planeModelMatrixFloat = planeModelMatrix.cast<float>();
    Eigen::Matrix4f sphereModelMatrixFloat = sphereModelMatrix.cast<float>();

    auto plainShader = shadersBucket.plainShader.get();
    auto planet3Shader = shadersBucket.planet3Shader.get();

    double visYTilesDelta = 1.0;
    double visXTilesDelta = 1.0;
    if (n == 1) {
        visYTilesDelta = 0.5;
        visXTilesDelta = 0.5;
    }
    if (n == 4) {
        visXTilesDelta = 2.0;
        visYTilesDelta = 2.0;
    }

    float textureTileSize = textureTileSizeUnit;
    bool forwardRenderingToWorld = zoom >= 4.6;

    float maxTilesZoom = mapControls.getMaxTilesZoom();
    if (zoom > maxTilesZoom) {
        visYTilesDelta = 0.5;
        visXTilesDelta = 1.0;
    }

    double camYNorm = (EPSG3857CamLatNorm - 1.0) / -2.0;
    double tileP = 1.0 / n;
    double camCenterYTile = camYNorm * n;
    double camYStart = std::fmax(0.0, camCenterYTile - visYTilesDelta);
    double camYEnd = std::fmin(n, camCenterYTile + visYTilesDelta);

    int visTileYStart = floor(camYStart);
    int visTileYEnd = ceil(camYEnd);
    int visTileYStartInv = n - visTileYEnd;
    int visTileYEndInv = n - visTileYStart;

    double camXNorm = (EPSG3857LonNormInf + 1.0) / 2.0;
    int yTilesAmount = visTileYEnd - visTileYStart;
    double camCenterXTile = camXNorm * n;
    double camXStart = camCenterXTile - visXTilesDelta;
    double camXEnd = camCenterXTile + visXTilesDelta;
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
    int segments = zoom > 7 ? 1 : 40;
    double planetVDelta = (planetVEnd - planetVStart) / segments;
    double verticesShift = planeSize / 2.0;

    double planetUCenter = 0.5;
    double planetUVDelta = tileP * visXTilesDelta;
    double planetUStart = std::fmax(planetUCenter - planetUVDelta, 0.0);
    double planetUEnd = std::fmin(planetUCenter + planetUVDelta, 1.0);
    double planetUDelta = (planetUEnd - planetUStart) / segments;

    double topPlanetV = planetVStart + segments * planetVDelta;
    double bottomPlanetV = planetVStart;
    double leftPlanetU = planetUStart;
    double rightPlanetU = planetUStart + segments * planetUDelta;

    float topYVertex = topPlanetV * planeSize - verticesShift;
    float leftXVertex = leftPlanetU * planeSize - verticesShift;
    float bottomYVertex = bottomPlanetV * planeSize - verticesShift;
    float rightXVertex = rightPlanetU * planeSize - verticesShift;

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

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    float topY = visTileYStart;
    float leftX = normalizeXTile(visTileXStartInf, n);
    //////////////////////////////////
    ////                            //
    ////       Render texture       //
    ////                            //
    //////////////////////////////////
    if (textureKey != newTextureKey && !forwardRenderingToWorld) {
        textureKey = newTextureKey;

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
                auto scaleM = EigenGL::createScaleMatrix(scale, scale, 1.0f);
                auto translate = EigenGL::createTranslationMatrix(translateX, translateY, 0);
                Eigen::Matrix4f vTileMatrix = viewTexture * translate * scaleM;
                Eigen::Matrix4f pvTileMatrix = projectionTexture * vTileMatrix;
                mapTileRender.renderTile(
                        shadersBucket,
                        backgroundTile,
                        mapCamera,
                        projectionTexture,
                        vTileMatrix,
                        pvTileMatrix,
                        zoom,
                        forwardRenderingToWorld
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
                int scissorX = ceil(translateXIndex * textureTileSize);
                int scissorY = ceil(yTilesAmount * textureTileSize - (translateYIndex + 1) * textureTileSize);
                glScissor(scissorX, scissorY, ceil(textureTileSize), ceil(textureTileSize));
                auto tileModelMatrix = EigenGL::createTranslationMatrix(translateX, translateY, 0);
                Eigen::Matrix4f vTileMatrix = viewTexture * tileModelMatrix;
                Eigen::Matrix4f pvTileMatrix = projectionTexture * vTileMatrix;
                mapTileRender.renderTile(
                        shadersBucket,
                        tile,
                        mapCamera,
                        projectionTexture,
                        vTileMatrix,
                        pvTileMatrix,
                        zoom,
                        forwardRenderingToWorld
                );
            }
        }
        glDisable(GL_SCISSOR_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        mapCamera.glViewportDeviceSize();
    }


    //////////////////////////////////
    ////                            //
    ////       Render planet        //
    ////                            //
    //////////////////////////////////
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (forwardRenderingToWorld) {
        double shiftXTileP = fmod(tilesSwiped, 1.0) + EPSGLonNormInfNegative;
        Eigen::Vector4d topLeftWorld4f = planeModelMatrix * Eigen::Vector4d(leftXVertex, topYVertex, 0, 1.0);
        Eigen::Vector4d bottomRightWorld4f = planeModelMatrix * Eigen::Vector4d(rightXVertex, bottomYVertex, 0, 1.0);
        Eigen::Vector3d topLeftWorld = topLeftWorld4f.head(3) / topLeftWorld4f.w();
        Eigen::Vector3d bottomRightWorld = bottomRightWorld4f.head(3) / bottomRightWorld4f.w();
        double worldTileSizeX = abs(bottomRightWorld.x() - topLeftWorld.x()) / (2.0 * visXTilesDelta);
        double worldTileSizeY = abs(bottomRightWorld.y() - topLeftWorld.y()) / yTilesAmount;
        double scaleTileX = worldTileSizeX / extent;
        double scaleTileY = worldTileSizeY / extent;

        Eigen::Vector4d bl = pv * planeModelMatrix * Eigen::Vector4d(leftXVertex, bottomYVertex, 0, 1.0);
        Eigen::Vector3d blNDC = bl.head(3) / bl.w();

        Eigen::Vector4d blNext = pv * planeModelMatrix * Eigen::Vector4d(rightXVertex, topYVertex, 0, 1.0);
        Eigen::Vector3d blNextNDC = blNext.head(3) / blNext.w();

        double halfScreenWidth = mapCamera.getScreenW() * 0.5f;
        double halfScreenHeight = mapCamera.getScreenH() * 0.5f;
        double viewportBLX = blNDC.x() * halfScreenWidth + halfScreenWidth;
        double viewportBLY = blNDC.y() * halfScreenHeight + halfScreenHeight;
        double viewportBLXNext = blNextNDC.x() * halfScreenWidth + halfScreenWidth;
        double viewportBLYNext = blNextNDC.y() * halfScreenHeight + halfScreenHeight;
        double viewportSizeX = viewportBLXNext - viewportBLX;
        double viewportSizeY = viewportBLYNext - viewportBLY;
        double viewportTileSizeX = viewportSizeX / (visXTilesDelta * 2.0);
        double viewportTileSizeY = viewportSizeY / yTilesAmount;

        glEnable(GL_SCISSOR_TEST);
        // рисуем фоновые тайлы
        for (int loop = -1; loop <= 1; loop++) {
            for (auto backgroundTile : backgroundTiles) {
                double deltaZ = tileZ - backgroundTile->getZ();
                double scale = pow(2.0, deltaZ);

                double translateXIndex = loop * n + (backgroundTile->getX() * scale - leftX) - shiftXTileP;
                double translateYIndex = (backgroundTile->getY() * scale - topY);

                double translateX = translateXIndex * worldTileSizeX + topLeftWorld.x();
                double translateY = translateYIndex * -worldTileSizeY + topLeftWorld.y();

                double scaleTileBgX = scale * scaleTileX;
                double scaleTileBgY = scale * scaleTileY;

                int scissorX = ceil(translateXIndex * viewportTileSizeX + viewportBLX);
                int scissorY = ceil(viewportBLY + yTilesAmount * viewportTileSizeY - (translateYIndex + 1.0 * scale) * viewportTileSizeY);
                int bgTileViewportSize = ceil(viewportTileSizeX * scale);
                if (scissorX + bgTileViewportSize < 0 || scissorX > mapCamera.getScreenW()) {
                    continue;
                }
                glScissor(scissorX, scissorY, bgTileViewportSize, bgTileViewportSize);

                auto scaleMatrix = EigenGL::createScaleMatrix(scaleTileBgX, scaleTileBgY, 1.0);
                auto translateMatrix = EigenGL::createTranslationMatrix(translateX, translateY, 0);
                Eigen::Matrix4d vTileMatrix = view * translateMatrix * scaleMatrix;
                Eigen::Matrix4d pvTileMatrix = projection * vTileMatrix;
                mapTileRender.renderTile(
                        shadersBucket,
                        backgroundTile,
                        mapCamera,
                        projection.cast<float>(),
                        vTileMatrix.cast<float>(),
                        pvTileMatrix.cast<float>(),
                        zoom,
                        forwardRenderingToWorld
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
                double translateXIndex = xPos;
                double translateYIndex = tileY - visTileYStart;
                double translateX = (translateXIndex - shiftXTileP) * worldTileSizeX + topLeftWorld.x();
                double translateY = translateYIndex * -worldTileSizeY + topLeftWorld.y();
                auto translateMatrix = EigenGL::createTranslationMatrix(translateX, translateY, 0);
                Eigen::Matrix4d scaleMatrix = EigenGL::createScaleMatrix(scaleTileX, scaleTileY, 1.0);
                Eigen::Matrix4d vTileMatrix = view * translateMatrix * scaleMatrix;
                Eigen::Matrix4d pvTileMatrix = projection * vTileMatrix;
                int scissorX = ceil(viewportBLX + (translateXIndex - shiftXTileP) * viewportTileSizeX);
                int scissorY = ceil(viewportBLY + yTilesAmount * viewportTileSizeY - (translateYIndex + 1.0) * viewportTileSizeY);
                glScissor(scissorX, scissorY, ceil(viewportTileSizeX), ceil(viewportTileSizeY));
                mapTileRender.renderTile(
                        shadersBucket,
                        tile,
                        mapCamera,
                        projection.cast<float>(),
                        vTileMatrix.cast<float>(),
                        pvTileMatrix.cast<float>(),
                        zoom,
                        forwardRenderingToWorld
                );
            }
        }
        glDisable(GL_SCISSOR_TEST);
    } else {
        std::vector<float> planetEPSG3857;
        std::vector<float> planetTexUV;
        std::vector<float> planetVertices;
        for (int i = 0; i <= segments; i ++) {
            double planetV = planetVStart + i * planetVDelta;
            float y = planetV * planeSize - verticesShift;

            for (int j = 0; j <= segments; j++) {
                double planetU = planetUStart + j * planetUDelta;
                float x = planetU * planeSize - verticesShift;

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

        glEnable(GL_DEPTH_TEST);
        glUseProgram(planet3Shader->program);
        glUniformMatrix4fv(planet3Shader->getMatrixLocation(), 1, GL_FALSE, pvFloat.data());
        glUniformMatrix4fv(planet3Shader->getPlaneMatrixLocation(), 1, GL_FALSE, planeModelMatrixFloat.data());
        glUniformMatrix4fv(planet3Shader->getSphereMatrixLocation(), 1, GL_FALSE, sphereModelMatrixFloat.data());
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


//    mapTest.drawCenterPoint(shadersBucket, pvFloat);
//    mapTest.drawTextureTest(shadersBucket, mapCamera, mapTexture, xTilesAmount, yTilesAmount);
    mapTest.drawFPS(shadersBucket, mapSymbols, mapCamera, mapFpsCounter.getFps());
}

void MapRenderer2::init(AAssetManager *assetManager, JNIEnv *env, jobject &request_tile) {
    mapTileGetter = new MapTileGetter(env, request_tile);
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
    // 78.236812, 15.623110
    float latitude = DEG2RAD(78.236812);
    float longitude = DEG2RAD(15.623110);

    mapControls.setCamPos(moscowLat, moscowLon);
    mapControls.setZoom(14);
    mapControls.initCamUnit(planeSize);
}


