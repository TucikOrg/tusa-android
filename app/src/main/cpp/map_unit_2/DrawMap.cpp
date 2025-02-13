//
// Created by Artem on 15.10.2024.
//

#include "DrawMap.h"
#include "Grid.h"
void DrawMap::onSurfaceCreated() {
    glGenBuffers(1, &verticesVBO);
    glGenBuffers(1, &indicesIBO);
    glGenBuffers(1, &texUV_VBO);
    glGenBuffers(1, &EPSG3857_VBO);
}

void DrawMap::destroy() {
    glDeleteBuffers(1, &verticesVBO);
    glDeleteBuffers(1, &indicesIBO);
    glDeleteBuffers(1, &texUV_VBO);
    glDeleteBuffers(1, &EPSG3857_VBO);
}

void DrawMap::drawMapForward(DrawMapData &data) {

    auto& forwardRenderingToWorld = data.forwardRenderingToWorld;
    auto& tilesSwiped = data.tilesSwiped;
    auto& EPSGLonNormInfNegative = data.EPSGLonNormInfNegative;
    auto& planeModelMatrix = data.planeModelMatrix;
    auto& leftXVertex = data.leftXVertex;
    auto& topYVertex = data.topYVertex;
    auto& rightXVertex = data.rightXVertex;
    auto& bottomYVertex = data.bottomYVertex;
    auto& yTilesAmount = data.yTilesAmount;
    auto& pv = data.pv;
    auto& mapCamera = data.mapCamera;
    auto& visXTilesDelta = data.visXTilesDelta;
    auto& backgroundTiles = data.backgroundTiles;
    auto& tiles = data.tiles;
    auto& tileZ = data.tileZ;
    auto& n = data.n;
    auto& leftX = data.leftX;
    auto& topY = data.topY;
    auto& mapTileRender = data.mapTileRender;
    auto& view = data.view;
    auto& projection = data.projection;
    auto& shadersBucket = data.shadersBucket;
    auto& zoom = data.zoom;
    auto& visTileYStart = data.visTileYStart;
    auto& visTileYEnd = data.visTileYEnd;
    auto& visTileXStartInf = data.visTileXStartInf;
    auto& visTileXEndInf = data.visTileXEndInf;
    auto& mapSymbols = data.mapSymbols;
    auto& mapFpsCounter = data.mapFpsCounter;
    auto& mapNumbers = data.mapNumbers;
    float scaleText = mapTileRender.getScaleText(mapNumbers);

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
        for (auto& backgroundTilePair : backgroundTiles) {
            auto backgroundTile = backgroundTilePair.second;
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
                    forwardRenderingToWorld,
                    mapSymbols,
                    mapNumbers
            );
        }
    }

    // рисуем актуальные тайлы
    // рисуем по тайлам. Тайл рисуем целиком и потом следующий тайл
    std::vector<OnTilePathText> onTilePathText = {};
    for (int tileY = visTileYStart; tileY < visTileYEnd; tileY++) {
        for (int tileXInf = visTileXStartInf, xPos = 0; tileXInf < visTileXEndInf; tileXInf++, xPos++) {
            int tileX = Utils::normalizeXTile(tileXInf, n);
            auto tileKey = MapTile::makeKey(tileX, tileY, tileZ);
            MapTile* tile = tiles[tileKey];
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
                    forwardRenderingToWorld,
                    mapSymbols,
                    mapNumbers,
                    mapFpsCounter.getTimeElapsed()
            );

            // для последующего рендринга текста
            onTilePathText.push_back(OnTilePathText {
                tile, scaleMatrix.cast<float>(), vTileMatrix.cast<float>(), pvTileMatrix.cast<float>()
            });
        }
    }
    glDisable(GL_SCISSOR_TEST);

    // Рисуем текст поверх всех тайлов
    if (zoom >= 14) {
//        mapTileRender.resetLettersRoadCollision();
//        mapTileRender.shouldRecalculateCollisions(mapFpsCounter.getTimeElapsed());
        if (mapTileRender.lockThread == false) {
            mapTileRender.savedVisTileYStart = mapNumbers.visTileYStart;
            mapTileRender.savedVisTileYEnd = mapNumbers.visTileYEnd;
            mapTileRender.savedVisTileXStartInf = mapNumbers.visTileXStartInf;
            mapTileRender.savedVisTileXEndInf = mapNumbers.visTileXEndInf;
            mapTileRender.savedN = mapNumbers.n;
            mapTileRender.savedTileZ = mapNumbers.tileZ;
            mapTileRender.savedTiles = tiles;
            mapTileRender.savedPVScreen = mapNumbers.pvScreen;
        }

        for (OnTilePathText &drawTile: onTilePathText) {
            auto tile = drawTile.mapTile;
            auto &vTileMatrix = drawTile.vTileMatrix;
            auto &pvTileMatrix = drawTile.pvTileMatrix;

            if (mapTileRender.lockThread == false) {
                tile->savedForRoadTextCollisionCheckPVM = pvTileMatrix;
                tile->savedScaleForRoadTextCollisionsCheck = scaleText;
            }

            mapTileRender.renderPathText(
                    tile,
                    mapSymbols,
                    vTileMatrix,
                    projection.cast<float>(),
                    shadersBucket,
                    mapNumbers,
                    mapFpsCounter.getTimeElapsed(),
                    pvTileMatrix,
                    scaleText
            );
        }
//        mapTileRender.setCheckRoadsCollisionsFalse();
    }
}


void DrawMap::drawMapViaTexture(DrawMapData &data) {
    auto& planeModelMatrix = data.planeModelMatrix;
    auto& pv = data.pv;
    auto& mapTileRender = data.mapTileRender;
    auto& shadersBucket = data.shadersBucket;
    auto& segments = data.segments;
    auto& planetVStart = data.planetVStart;
    auto& planetUStart = data.planetUStart;
    auto& planetVDelta = data.planetVDelta;
    auto& planetUDelta = data.planetUDelta;
    auto& planeSize = data.planeSize;
    auto& verticesShift = data.verticesShift;
    auto& sphereModelMatrixFloat = data.sphereModelMatrixFloat;
    auto& transition = data.transition;
    auto& EPSG3857CamLat = data.EPSG3857CamLat;
    auto& shiftUTex = data.shiftUTex;
    auto& scaleUTex = data.scaleUTex;


    if(segments != savedSegmentsAmount || data.tileZ != savedTileZ) {
        savedSegmentsAmount = segments;
        savedTileZ = data.tileZ;
        std::vector<float> planetEPSG3857;
        std::vector<float> planetTexUV;
        std::vector<float> planetVertices;
        std::vector<unsigned int> indices;

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

        glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
        glBufferData(GL_ARRAY_BUFFER, planetVertices.size() * sizeof(float), planetVertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, EPSG3857_VBO);
        glBufferData(GL_ARRAY_BUFFER, planetEPSG3857.size() * sizeof(float), planetEPSG3857.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, texUV_VBO);
        glBufferData(GL_ARRAY_BUFFER, planetTexUV.size() * sizeof(float), planetTexUV.data(), GL_STATIC_DRAW);

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

        indicesSize = indices.size();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }

    auto planet3Shader = shadersBucket.planet3Shader.get();
    Eigen::Matrix4f pvFloat = pv.cast<float>();
    Eigen::Matrix4f planeModelMatrixFloat = planeModelMatrix.cast<float>();

    glEnable(GL_DEPTH_TEST);
    glUseProgram(planet3Shader->program);
    glUniformMatrix4fv(planet3Shader->getMatrixLocation(), 1, GL_FALSE, pvFloat.data());
    glUniformMatrix4fv(planet3Shader->getPlaneMatrixLocation(), 1, GL_FALSE, planeModelMatrixFloat.data());
    glUniformMatrix4fv(planet3Shader->getSphereMatrixLocation(), 1, GL_FALSE, sphereModelMatrixFloat.data());
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glVertexAttribPointer(planet3Shader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(planet3Shader->getPosLocation());
    glBindBuffer(GL_ARRAY_BUFFER, EPSG3857_VBO);
    glVertexAttribPointer(planet3Shader->getPlanetEPSG3857Location(), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(planet3Shader->getPlanetEPSG3857Location());
    glBindBuffer(GL_ARRAY_BUFFER, texUV_VBO);
    glVertexAttribPointer(planet3Shader->getTextureUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(planet3Shader->getTextureUVLocation());
    glUniform1i(planet3Shader->getTextureLocation(), 0);
    glUniform1f(planet3Shader->getPlaneSizeLocation(), planeSize);
    glUniform1f(planet3Shader->getTransitionLocation(), transition);
    glUniform2f(planet3Shader->getCameraEPSG3857Location(), EPSG3857CamLat, 0.0f);
    glUniform2f(planet3Shader->getUVOffsetLocation(), shiftUTex, 0.0f);
    glUniform2f(planet3Shader->getUVScaleLocation(), scaleUTex, 1.0f);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesIBO);
    glDrawElements(GL_TRIANGLE_STRIP, indicesSize, GL_UNSIGNED_INT, 0);
    glDisable(GL_DEPTH_TEST);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
