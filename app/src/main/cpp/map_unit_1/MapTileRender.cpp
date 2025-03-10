//
// Created by Artem on 16.09.2024.
//

#include "MapTileRender.h"
#include "shader/shaders_bucket.h"
#include "util/android_log.h"
#include "util/eigen_gl.h"
#include "MapColors.h"
#include "RoadLettersPtr.h"

float MapTileRender::getScaleText(MapNumbers& mapNumbers) {
    float scaleText = mapNumbers.scale * 0.1;
    float scaleUnit = 0.5;

    if (mapNumbers.tileZ == 14) {
        scaleText *= scaleUnit;
    } else if (mapNumbers.tileZ == 15) {
        scaleText *= scaleUnit * 3;
    } else if (mapNumbers.tileZ == 16) {
        scaleText *= scaleUnit * 6;
    }
    return scaleText;
}

MapTileRender::MapTileRender() {
    parallelThread = std::thread([this] {
        while (parallelThreadRunning) {
            lockThread = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
            if (parallelThreadRunning == false) break;
            lockThread = true;

            if (savedTiles.empty() || savedTileZ < 14) continue;
            resetLettersRoadCollision();

            auto& visTileYStart = savedVisTileYStart;
            auto& visTileYEnd = savedVisTileYEnd;
            auto& visTileXStartInf = savedVisTileXStartInf;
            auto& visTileXEndInf = savedVisTileXEndInf;
            auto& n = savedN;
            auto& tileZ = savedTileZ;
            auto& tiles = savedTiles;
            for (int tileY = visTileYStart; tileY < visTileYEnd; tileY++) {
                for (int tileXInf = visTileXStartInf, xPos = 0;
                     tileXInf < visTileXEndInf; tileXInf++, xPos++) {
                    int tileX = Utils::normalizeXTile(tileXInf, n);
                    auto tileKey = MapTile::makeKey(tileX, tileY, tileZ);
                    MapTile *tile = tiles[tileKey];
                    if (tile->isEmpty()) {
                        continue;
                    }
                    float savedScaleText = tile->savedScaleForRoadTextCollisionsCheck;

                    for (auto& drawTextItem : tile->resultDrawTextAlongPath) {
                        auto& path = drawTextItem.path;
                        auto pathLen = drawTextItem.legthOfPath;
                        float textWidth = drawTextItem.textWidth * savedScaleText;
                        // текст гораздо боьше этого маленького отрезка пути
                        if (textWidth + textCollisionDelta > pathLen) continue;
                        float startShift = (pathLen - textWidth) / 2;
                        float yShift = drawTextItem.textHeight / 4.0;
                        auto forRender = drawTextItem.forRender;

                        // проверяем коллизию с другими буквами
                        bool collisionPassed = true;
                        std::vector<float> tempLettersScreenXY = {};
                        float currentShift = 0;
                        for (auto charRender: forRender) {
                            Symbol symbol = std::get<0>(charRender);
                            float w = std::get<1>(charRender);
                            float h = std::get<2>(charRender);
                            float pixelsShift = std::get<3>(charRender);
                            float bitmapLeft = symbol.bitmapLeft;
                            float rowsMinusTop = (symbol.rows - symbol.bitmapTop) + yShift;

                            // здесь вычисляем позицию каждой буквы
                            size_t u_pointsSize = path.size();
                            float skipLen = currentShift * savedScaleText + startShift;
                            float pointX, pointY;
                            float nextPointX, nextPointY;
                            for (int i = 0; i < u_pointsSize - 1; i++) {
                                pointX = path[i * 2];
                                pointY = path[i * 2 + 1];
                                nextPointX = path[i * 2 + 2];
                                nextPointY = path[i * 2 + 2 + 1];
                                float dist = sqrt( pow(nextPointX - pointX, 2.0) + pow(nextPointY - pointY, 2.0) );
                                if (skipLen > dist) {
                                    skipLen -= dist;
                                } else break;
                            }
                            float fromPointShift = skipLen - pixelsShift * savedScaleText / 2.0;

                            Eigen::Vector2f tangent = Eigen::Vector2f(nextPointX, nextPointY) - Eigen::Vector2f(pointX, pointY);
                            tangent.normalize();
                            Eigen::Vector2f orthogonal = Eigen::Vector2f(-tangent.y(), tangent.x());

                            Eigen::Vector2f point = Eigen::Vector2f(pointX, pointY);
                            Eigen::Vector2f vertexPosition = Eigen::Vector2f (w / 2.0f, h / 2.0f);
                            Eigen::Vector2f a_symbolShift = Eigen::Vector2f (bitmapLeft, rowsMinusTop);
                            Eigen::Vector2f startLoc = (vertexPosition.x() + a_symbolShift.x()) * tangent + (vertexPosition.y() - a_symbolShift.y()) * orthogonal;
                            startLoc *= savedScaleText;
                            Eigen::Vector2f symbPos = startLoc + point + tangent * fromPointShift;
                            float symbPosX = symbPos.x();
                            float symbPosY = symbPos.y();

                            Eigen::Vector4f gomogenCoordinate = tile->savedForRoadTextCollisionCheckPVM * Eigen::Vector4f(symbPosX, symbPosY, 0, 1);
                            float ndcX = gomogenCoordinate.x() / gomogenCoordinate.w();
                            float ndcY = gomogenCoordinate.y() / gomogenCoordinate.w();
                            float screenX = (ndcX + 1) * 0.5 * screenWidth;
                            float screenY = (1.0 - (ndcY + 1) * 0.5) * screenHeight;
                            Eigen::Matrix4f pvScreen = savedPVScreen;

                            // проверяем ппересечения с другими буквами
                            for (auto textRoad : roadLettersPtr) {
                                if (
                                        textRoad.roadId == drawTextItem.roadId &&
                                        textRoad.regionId != drawTextItem.regionId &&
                                        textRoad.tileKey != drawTextItem.tileId
                                        ) continue;

                                for (int letterI = 0; letterI < textRoad.amount; letterI++) {
                                    float otherLetterX = roadLettersScreenXY[textRoad.startIndex + letterI * 2];
                                    float otherLetterY = roadLettersScreenXY[textRoad.startIndex + letterI * 2 + 1];

                                    float deltaX = screenX - otherLetterX;
                                    float deltaY = screenY - otherLetterY;
                                    float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

                                    if (distance < 35) {
                                        collisionPassed = false;
                                        break;
                                    }
                                }

                                if (collisionPassed == false) break;
                            }
                            if (collisionPassed == false) break;

                            tempLettersScreenXY.push_back(screenX);
                            tempLettersScreenXY.push_back(screenY);

                            currentShift += pixelsShift;
                        }

                        if (collisionPassed) {
                            uint startIndexLetters = roadLettersScreenXY.size();
                            // если рендрим в итоге то добавляем
                            for (auto coord : tempLettersScreenXY) {
                                roadLettersScreenXY.push_back(coord);
                            }
                            uint lettersAmount = tempLettersScreenXY.size() / 2;
                            roadLettersPtr.push_back(RoadLettersPtr { drawTextItem.roadId, tileKey, startIndexLetters, lettersAmount, drawTextItem.regionId, } );
                        }

                        // посчитали коллизию и сохранили в объект текста дороги
                        drawTextItem.collisionPassed = collisionPassed;
                    }
                }
            }

        }
    });
};

void MapTileRender::initTilesTexture() {
    glGenFramebuffers(1, &tilesFrameBuffer);
    glGenTextures(1, &tilesTexture);
    glBindTexture(GL_TEXTURE_2D, tilesTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &mapTextureFramebuffer);
    glGenTextures(1, &mapTexture);
    glBindTexture(GL_TEXTURE_2D, mapTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void MapTileRender::renderTexture(
        RenderTextureData data,
        MapCamera& mapCamera,
        ShadersBucket& shadersBucket
) {
    auto textureTileSize = data.textureTileSize;
    auto  xTilesAmount = data.xTilesAmount;
    auto  yTilesAmount = data.yTilesAmount;
    auto  backgroundTiles = data.backgroundTiles;
    auto tiles = data.tiles;
    auto forwardRenderingToWorld = data.forwardRenderingToWorld;
    auto tileZ = data.tileZ;
    auto zoom = data.zoom;
    auto n = data.n;
    auto leftX = data.leftX;
    auto topY = data.topY;
    auto visTileYStart = data.visTileYStart;
    auto visTileYEnd = data.visTileYEnd;
    auto visTileXStartInf = data.visTileXStartInf;
    auto visTileXEndInf = data.visTileXEndInf;


    float textureWidth = textureTileSize * xTilesAmount;
    float textureHeight = textureTileSize * yTilesAmount;
    if (prTex2dHeight != textureHeight || prTex2dWidth != textureWidth) {
        glBindTexture(GL_TEXTURE_2D, mapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        prTex2dHeight = textureHeight;
        prTex2dWidth = textureWidth;
        //LOGI("New texture size %f %f", textureWidth, textureHeight);
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
        for (auto& backgroundTilePair : backgroundTiles) {
            auto backgroundTile = backgroundTilePair.second;
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
            renderTile(
                    shadersBucket,
                    backgroundTile,
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
            int tileX = Utils::normalizeXTile(tileXInf, n);
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
            renderTile(
                    shadersBucket,
                    tile,
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

void MapTileRender::renderPathText(MapTile* tile, MapSymbols& mapSymbols,
                                   Eigen::Matrix4f vm, Eigen::Matrix4f p,
                                   ShadersBucket& shadersBucket,
                                   MapNumbers& mapNumbers,
                                   float elapsedTime,
                                   Eigen::Matrix4f pvm,
                                   float scaleText
) {
    auto atlasW = mapSymbols.atlasWidth;
    auto atlasH = mapSymbols.atlasHeight;
    auto& drawTextAlongPath= tile->resultDrawTextAlongPath;


    // тестирование
    // рисуем точки по которым идет текст
//    auto plainShader = shadersBucket.plainShader;
//    for (auto& drawTextItem: drawTextAlongPath) {
//        auto randomColor = drawTextItem.color;
//        std::vector<float> lines = {};
//        std::vector<unsigned int> indicesLine = {};
//
//        for (auto point : drawTextItem.points) {
//            float data1[] = { FLOAT(point.x), FLOAT(-point.y) };
//            lines.push_back(FLOAT(point.x));
//            lines.push_back(FLOAT(-point.y));
//        }
//
//        for (int i = 0; i < drawTextItem.points.size(); i++) {
//            indicesLine.push_back(i);
//            if (i != 0 && i != drawTextItem.points.size() - 1) {
//                indicesLine.push_back(i);
//            }
//        }
//
//        glUseProgram(plainShader->program);
//        glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, lines.data());
//        glEnableVertexAttribArray(plainShader->getPosLocation());
//        glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
//        glUniform1f(plainShader->getPointSizeLocation(), 40.0f);
//        glUniform4f(plainShader->getColorLocation(), randomColor[0], randomColor[1], randomColor[2], 1.0);
//        glLineWidth(8.0f);
//        glDrawElements(GL_LINES, indicesLine.size(), GL_UNSIGNED_INT, indicesLine.data());
//        glDrawArrays(GL_POINTS, 0, lines.size() / 2);
//    }

    auto pathTextShader = shadersBucket.pathTextShader;

    for (int drIndex = 0; drIndex < drawTextAlongPath.size(); drIndex++) {
        auto& drawTextItem = drawTextAlongPath[drIndex];
        auto& path = drawTextItem.path;
        auto pathLen = drawTextItem.legthOfPath;
        float textWidth = drawTextItem.textWidth * scaleText;
        // текст гораздо боьше этого маленького отрезка пути
        if (textWidth + textCollisionDelta > pathLen) continue;
        float startShift = (pathLen - textWidth) / 2;
        float yShift = drawTextItem.textHeight / 4.0;
        auto forRender = drawTextItem.forRender;

        short dataElementsCount = 8;
        if (drawTextItem.transferedToGPU == false) {
            float currentShift = 0;
            unsigned int symbolDataCurrentIndex = 0;

            unsigned int symbolDataSize = forRender.size() * 4 * dataElementsCount;
            std::vector<float> symbolData = std::vector<float>(symbolDataSize);
            for (auto charRender : forRender) {
                Symbol symbol = std::get<0>(charRender);
                float w = std::get<1>(charRender);
                float h = std::get<2>(charRender);
                float pixelsShift = std::get<3>(charRender);
                float bitmapLeft = symbol.bitmapLeft;
                float rowsMinusTop = (symbol.rows - symbol.bitmapTop) + yShift;
                auto startU = symbol.startU(atlasW);
                auto endU = symbol.endU(atlasW);
                auto startV = symbol.startV(atlasH);
                auto endV = symbol.endV(atlasH);

                float symbolArray[] = {
                        0, 0, currentShift, pixelsShift, bitmapLeft, rowsMinusTop, startU, startV,
                        w, 0, currentShift, pixelsShift, bitmapLeft, rowsMinusTop, endU, startV,
                        w, h, currentShift, pixelsShift, bitmapLeft, rowsMinusTop, endU, endV,
                        0, h, currentShift, pixelsShift, bitmapLeft, rowsMinusTop, startU, endV
                };
                for (auto& item : symbolArray) {
                    symbolData[symbolDataCurrentIndex++] = item;
                }

                currentShift += pixelsShift;
            }

            std::vector<unsigned int> indices = std::vector<unsigned int>(forRender.size() * 6);
            for (int i = 0; i < forRender.size(); i++) {
                unsigned int pointsShift = i * 4;
                unsigned int indexShift = i * 6;
                indices[indexShift + 0] = 0 + pointsShift;
                indices[indexShift + 1] = 1 + pointsShift;
                indices[indexShift + 2] = 3 + pointsShift;
                indices[indexShift + 3] = 1 + pointsShift;
                indices[indexShift + 4] = 2 + pointsShift;
                indices[indexShift + 5] = 3 + pointsShift;
            }

            drawTextItem.iboSize = indices.size();
            glGenBuffers(1, &drawTextItem.vboData);
            glBindBuffer(GL_ARRAY_BUFFER, drawTextItem.vboData);
            glBufferData(GL_ARRAY_BUFFER, symbolData.size() * sizeof(float), symbolData.data(), GL_STATIC_DRAW);

            glGenBuffers(1, &drawTextItem.ibo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawTextItem.ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

            drawTextItem.transferedToGPU = true;
        }

        // не прошел проверку на пересечения -> игнорируем его
        if (drawTextItem.collisionPassed == false) continue;

        glUseProgram(pathTextShader->program);
        glUniformMatrix4fv(pathTextShader->getProjectionMatrix(), 1, GL_FALSE, p.data());
        glUniform4f(pathTextShader->getColorLocation(), 1.0, 0.0, 0.0, 1.0);
        glUniform1i(pathTextShader->getTextureLocation(), 0);
        glUniformMatrix4fv(pathTextShader->getMatrixLocation(), 1, GL_FALSE, vm.data());
        glUniform1f(pathTextShader->getElapsedTimeLocation(), elapsedTime);
        glUniform1f(pathTextShader->getScaleLocation(), scaleText);

        // Рисуем текст на дороге
        glUniform2fv(pathTextShader->getPathLocation(), path.size(), path.data());
        glUniform1i(pathTextShader->getPointsSizeLocation(), path.size());
        glUniform1f(pathTextShader->getStartShiftLocation(), startShift);
        glBindBuffer(GL_ARRAY_BUFFER, drawTextItem.vboData);
        int stride = dataElementsCount * sizeof(float);
        glVertexAttribPointer(pathTextShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, stride, 0);
        glEnableVertexAttribArray(pathTextShader->getPosLocation());
        glVertexAttribPointer(pathTextShader->getSkipLenLocation(), 1, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(pathTextShader->getSkipLenLocation());
        glVertexAttribPointer(pathTextShader->getPixelShiftLocation(), 1, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(pathTextShader->getPixelShiftLocation());
        glVertexAttribPointer(pathTextShader->getSymbolShiftLocation(), 2, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(float)));
        glEnableVertexAttribArray(pathTextShader->getSymbolShiftLocation());
        glVertexAttribPointer(pathTextShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(pathTextShader->getTextureCord());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawTextItem.ibo);
        glUniform1f(pathTextShader->getSymbolScaleLocation(), 3.0);
        glUniform3f(pathTextShader->getColorLocation(), 1.0, 1.0, 1.0);
        glDrawElements(GL_TRIANGLES, drawTextItem.iboSize, GL_UNSIGNED_INT, 0);
        glUniform1f(pathTextShader->getSymbolScaleLocation(), 0.0);
        glUniform3f(pathTextShader->getColorLocation(), 0.0, 0.0, 0.0);
        glDrawElements(GL_TRIANGLES, drawTextItem.iboSize, GL_UNSIGNED_INT, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }


}

void MapTileRender::renderTile(
        ShadersBucket &shadersBucket,
        MapTile *tile,
        Eigen::Matrix4f p,
        Eigen::Matrix4f vm,
        Eigen::Matrix4f pvm,
        float zoom,
        bool isForwardRendering,
        unsigned short from,
        unsigned short to
) {
    auto styles = tile->style.getStylesVec();
    auto stylesSize = styles.size();
    if (to == 0)
        to = stylesSize;

    if (from == 0) {
        drawBackground(shadersBucket, pvm);
    }

    for (unsigned short styleIndex = from; styleIndex < to; styleIndex++) {
        auto style = styles[styleIndex];
        drawLayer(
                shadersBucket,
                tile, p, vm, pvm,
                style, zoom,
                isForwardRendering
        );
    }
}

GLuint MapTileRender::getTilesTexture() {
    return tilesTexture;
}

void MapTileRender::drawLayer(
        ShadersBucket& shadersBucket,
        MapTile* tile,
        Eigen::Matrix4f p,
        Eigen::Matrix4f vm,
        Eigen::Matrix4f pvm,
        int styleIndex,
        float zoom,
        bool isForwardRendering
) {
    float scaleFactor = pow(2, zoom);
    auto& style = tile->style;
    bool isWideLine = style.getIsWideLine(styleIndex);
    float renderWideAfter = style.getRenderWideAfterZoom(styleIndex);
    bool canDrawWideLines = zoom > renderWideAfter;
    auto plainShader = shadersBucket.plainShader;
    auto roadShader = shadersBucket.roadShader;
    auto simplePointsShader = shadersBucket.simplePointShader;
    float lineWidth = style.getLineWidth(styleIndex);
    float wideLineWidth = style.getLineWidth(styleIndex);

    // после определенного зума нужно дороги скейлить чтобы они уменьшались
    float zoomBorderFrom = 14.9;
    float zoomBorderTo = 17.0;
    float maxDecreaseWideLineLevel = 0.7;
    if (zoom >= zoomBorderFrom) {
        float betweenZoomsDelta = zoomBorderTo - zoomBorderFrom;
        float factor = 1.0 - std::min((zoom - zoomBorderFrom) / betweenZoomsDelta, 1.0f) * maxDecreaseWideLineLevel;
        //LOGI("Factor = %f", factor);

        wideLineWidth *= factor;
    }

    float borderFactor = style.getBorderFactor(styleIndex);
    auto color = CommonUtils::toOpenGlColor(style.getColor(styleIndex));
    auto borderColor = CommonUtils::toOpenGlColor(style.getBorderColor(styleIndex));
    auto& polygons = tile->resultPolygonsAggregatedByStyles[styleIndex];
    auto& lines = tile->resultLinesAggregatedByStyles[styleIndex];
    auto visibleZoom = style.getVisibleZoom(styleIndex);
    auto forwardRenderingOnly = style.getForwardRenderingOnly(styleIndex);
    bool exitByFR = forwardRenderingOnly && !isForwardRendering;
    bool exitByVisZoom = visibleZoom.find(floor(zoom)) == visibleZoom.end();
    if (exitByVisZoom || exitByFR) {
        return;
    }

    auto colorData = color.data();
    auto alphaFrom = style.getAlphaInterpolateFrom(styleIndex);
    auto alphaTo = style.getAlphaInterpolateTo(styleIndex);
    if (alphaFrom != -1.0 && alphaTo != -1.0) {
        float alpha = 0.0;
        if (zoom > alphaFrom && zoom < alphaTo) {
            alpha = 1.0 - (alphaTo - zoom) / (alphaTo - alphaFrom);
        }
        if (zoom >= alphaTo) {
            alpha = 1.0;
        }
        colorData[3] = alpha;
    }

    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
    if (lineWidth != 0)  {
        // делаем линии тоньше на маленьких зумах и темнее
        if (zoom <= 5) {
            lineWidth *= 0.3;
        } else if (zoom == 6) {
            lineWidth *= 0.4;
        } else if (zoom == 7) {
            lineWidth *= 0.5;
        } else if (zoom == 8) {
            lineWidth *= 0.6;
        } else if (zoom == 9) {
            lineWidth *= 0.7;
        }
        glLineWidth(lineWidth);
    }
    glUniform4fv(plainShader->getColorLocation(), 1, colorData);


    if (polygons.canBeDraw()) {
        glBindBuffer(GL_ARRAY_BUFFER, polygons.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, polygons.ibo);

        glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(plainShader->getPosLocation());
        glDrawElements(GL_TRIANGLES, polygons.iboSize, GL_UNSIGNED_INT, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    bool drawWideLines = canDrawWideLines && isWideLine;
    if (lines.canBeDraw() && !drawWideLines) {
        glBindBuffer(GL_ARRAY_BUFFER, lines.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lines.ibo);

        glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT,
                              GL_FALSE, 0, 0
        );
        glEnableVertexAttribArray(plainShader->getPosLocation());
        glDrawElements(GL_LINES, lines.iboSize, GL_UNSIGNED_INT, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    auto& wideLines = tile->resultWideLineAggregatedByStyles[styleIndex];
    if (wideLines.canBeDraw() && drawWideLines) {
        glUseProgram(roadShader->program);
        glUniform1f(roadShader->getWidthLocation(), wideLineWidth);
        glUniform1f(roadShader->getBorderFactorLocation(), borderFactor);
        glUniformMatrix4fv(roadShader->getMatrixLocation(), 1, GL_FALSE, vm.data());
        glUniformMatrix4fv(roadShader->getProjectionLocation(), 1, GL_FALSE, p.data());
        glBindBuffer(GL_ARRAY_BUFFER, wideLines.vbo);
        glVertexAttribPointer(roadShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(roadShader->getPosLocation());
        glBindBuffer(GL_ARRAY_BUFFER, wideLines.vboPerpendiculars);
        glVertexAttribPointer(roadShader->getPerpendicularsLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(roadShader->getPerpendicularsLocation());
        glBindBuffer(GL_ARRAY_BUFFER, wideLines.vboUv);
        glVertexAttribPointer(roadShader->getUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(roadShader->getUVLocation());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wideLines.ibo);

        // рисуем сначала только границы дорог, без заливки
        auto savedColorAlpha = colorData[3];
        colorData[3] = 0.0f;
        glUniform4fv(roadShader->getColorLocation(), 1, colorData);
        glUniform4fv(roadShader->getBorderColorLocation(), 1, borderColor.data());
        glDrawElements(GL_TRIANGLES, wideLines.iboSize, GL_UNSIGNED_INT, 0);
        colorData[3] = savedColorAlpha;

        // теперь рисуем заливку дорог
        auto savedBorderAlpha = borderColor[3];
        borderColor[3] = 0.0f;
        glUniform4fv(roadShader->getColorLocation(), 1, colorData);
        glUniform4fv(roadShader->getBorderColorLocation(), 1, borderColor.data());
        glDrawElements(GL_TRIANGLES, wideLines.iboSize, GL_UNSIGNED_INT, 0);
        borderColor[3] = savedBorderAlpha;

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    float simplePointSize = 50.0;
    auto simplePoints = tile->resultPointsAggregatedByStyles[styleIndex];
    if (simplePoints.vertices.size() > 0) {
        glUseProgram(simplePointsShader->program);
        glUniformMatrix4fv(simplePointsShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
        glUniform1f(simplePointsShader->getPointSizeLocation(), simplePointSize);
        glUniform4fv(simplePointsShader->getColorLocation(), 1, colorData);
        glVertexAttribPointer(simplePointsShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, simplePoints.vertices.data());
        glEnableVertexAttribArray(simplePointsShader->getPosLocation());
        glDrawArrays(GL_POINTS, 0, simplePoints.vertices.size() / 2);
    }

    auto squarePoints = tile->resultSquarePointsAggregatedByStyles[styleIndex];
    auto squarePointsShader = shadersBucket.squarePointShader;
    if (squarePoints.canBeDraw() && drawWideLines) {
        glUseProgram(squarePointsShader->program);
        glUniformMatrix4fv(squarePointsShader->getMatrixLocation(), 1, GL_FALSE, vm.data());
        glUniformMatrix4fv(squarePointsShader->getProjectionLocation(), 1, GL_FALSE, p.data());
        glUniform1f(squarePointsShader->getPointSizeLocation(), wideLineWidth - borderFactor * wideLineWidth * 2.0);
        glUniform4fv(squarePointsShader->getColorLocation(), 1, colorData);
        glBindBuffer(GL_ARRAY_BUFFER, squarePoints.vbo);
        glVertexAttribPointer(squarePointsShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(squarePointsShader->getPosLocation());
        glBindBuffer(GL_ARRAY_BUFFER, squarePoints.vboUVs);
        glVertexAttribPointer(squarePointsShader->getUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, squarePoints.uvs.data());
        glEnableVertexAttribArray(squarePointsShader->getUVLocation());
        glBindBuffer(GL_ARRAY_BUFFER, squarePoints.vboShifts);
        glVertexAttribPointer(squarePointsShader->getShiftLocation(), 2, GL_FLOAT, GL_FALSE, 0, squarePoints.shifts.data());
        glEnableVertexAttribArray(squarePointsShader->getShiftLocation());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squarePoints.ibo);
        glDrawElements(GL_TRIANGLES, squarePoints.iboSize, GL_UNSIGNED_INT, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }


    // тестирование
//    if (wideLines.canBeDraw() && drawWideLines) {
//        // рисуем точки границы дороги
//        glUseProgram(roadShader->program);
//        glUniform1f(roadShader->getWidthLocation(), wideLineWidth);
//        glUniform1f(roadShader->getBorderFactorLocation(), borderFactor);
//        glUniformMatrix4fv(roadShader->getMatrixLocation(), 1, GL_FALSE, vm.data());
//        glUniformMatrix4fv(roadShader->getProjectionLocation(), 1, GL_FALSE, p.data());
//        glUniform4fv(roadShader->getColorLocation(), 1, colorData);
//        glUniform4fv(roadShader->getBorderColorLocation(), 1, borderColor.data());
//        glBindBuffer(GL_ARRAY_BUFFER, wideLines.vbo);
//        glVertexAttribPointer(roadShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
//        glEnableVertexAttribArray(roadShader->getPosLocation());
//        glBindBuffer(GL_ARRAY_BUFFER, wideLines.vboPerpendiculars);
//        glVertexAttribPointer(roadShader->getPerpendicularsLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
//        glEnableVertexAttribArray(roadShader->getPerpendicularsLocation());
//        glBindBuffer(GL_ARRAY_BUFFER, wideLines.vboUv);
//        glVertexAttribPointer(roadShader->getUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
//        glEnableVertexAttribArray(roadShader->getUVLocation());
//        glUniform1f(roadShader->getPointSizeLocation(), 20.0f);
//        glUniform4f(roadShader->getColorLocation(), 1.0f, 0.0f, 0.0f, 1.0f);
//        glDrawArrays(GL_POINTS, 0, wideLines.verticesSize / 2.0);
//
//        glUseProgram(plainShader->program);
//        glUniform1f(plainShader->getPointSizeLocation(), 20.0f);
//        glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
//        glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
//        glEnableVertexAttribArray(plainShader->getPosLocation());
//        glUniform4f(plainShader->getColorLocation(), 1.0f, 0.0f, 0.0f, 1.0f);
//        glDrawArrays(GL_POINTS, 0, wideLines.verticesSize / 2.0);
//
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//    }
//
//    // тестовые точки
//    // GREEN POINTS is last points
//    glUseProgram(plainShader->program);
//    glUniform1f(plainShader->getPointSizeLocation(), 40.0f);
//    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
//    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, tile->lastPointsTest.data());
//    glEnableVertexAttribArray(plainShader->getPosLocation());
//    glUniform4f(plainShader->getColorLocation(), 0.0f, 1.0f, 0.0f, 1.0f);
//    glDrawArrays(GL_POINTS, 0, tile->lastPointsTest.size() / 2.0);
//
//    // BLUE POINTS is first points
//    glUseProgram(plainShader->program);
//    glUniform1f(plainShader->getPointSizeLocation(), 20.0f);
//    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
//    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, tile->firstPointsTest.data());
//    glEnableVertexAttribArray(plainShader->getPosLocation());
//    glUniform4f(plainShader->getColorLocation(), 0.0f, 0.0f, 1.0f, 1.0f);
//    glDrawArrays(GL_POINTS, 0, tile->firstPointsTest.size() / 2.0);
}

void MapTileRender::drawBackground(
        ShadersBucket& shadersBucket,
        Eigen::Matrix4f matrix
) {
    auto plainShader = shadersBucket.plainShader;
    float extent = 4096;
    std::vector<float> vertices = {
            0, 0,
            0, -extent,
            extent, -extent,
            extent, 0,
    };
    auto backgroundColor = CommonUtils::toOpenGlColor(MapColors::getBackground());
    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, matrix.data());
    glUniform4fv(plainShader->getColorLocation(), 1.0f, backgroundColor.data());
    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}


void MapTileRender::destroy() {
    glDeleteFramebuffers(1, &tilesFrameBuffer);
    glDeleteTextures(1, &tilesTexture);
    glDeleteFramebuffers(1, &mapTextureFramebuffer);
    glDeleteTextures(1, &mapTexture);
}









