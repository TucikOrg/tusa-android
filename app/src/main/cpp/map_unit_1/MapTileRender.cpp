//
// Created by Artem on 16.09.2024.
//

#include "MapTileRender.h"
#include "shader/shaders_bucket.h"
#include "util/android_log.h"
#include "util/eigen_gl.h"
#include "MapColors.h"

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

void MapTileRender::renderTexture(RenderTextureData &data) {
    auto& textureTileSize = data.textureTileSize;
    auto&  xTilesAmount = data.xTilesAmount;
    auto&  yTilesAmount = data.yTilesAmount;
    auto& mapCamera = data.mapCamera;
    auto&  backgroundTiles = data.backgroundTiles;
    auto& tiles = data.tiles;
    auto& forwardRenderingToWorld = data.forwardRenderingToWorld;
    auto& tileZ = data.tileZ;
    auto& zoom = data.zoom;
    auto& shadersBucket = data.shadersBucket;
    auto& n = data.n;
    auto& leftX = data.leftX;
    auto& topY = data.topY;
    auto& visTileYStart = data.visTileYStart;
    auto& visTileYEnd = data.visTileYEnd;
    auto& visTileXStartInf = data.visTileXStartInf;
    auto& visTileXEndInf = data.visTileXEndInf;
    auto& mapSymbols = data.mapSymbols;
    auto& mapNumbers = data.mapNumbers;

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
                    mapCamera,
                    projectionTexture,
                    vTileMatrix,
                    pvTileMatrix,
                    zoom,
                    forwardRenderingToWorld,
                    mapSymbols,
                    mapNumbers
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
                    mapCamera,
                    projectionTexture,
                    vTileMatrix,
                    pvTileMatrix,
                    zoom,
                    forwardRenderingToWorld,
                    mapSymbols,
                    mapNumbers
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
                                   MapNumbers& mapNumbers, float elapsedTime) {
    float symbolScale = 2.0;
//    if (mapNumbers.zoom >= 16 && mapNumbers.zoom <= 17) {
//        symbolScale = 3.0 - (mapNumbers.zoom - 16) * 2.0;
//    } else if (mapNumbers.zoom > 17) {
//        symbolScale = 1.0;
//    }

    auto atlasW = mapSymbols.atlasWidth;
    auto atlasH = mapSymbols.atlasHeight;
    auto color = CSSColorParser::parse("rgb(0, 0, 0)");
    auto drawTextAlongPath = tile->resultDrawTextAlongPath;
    glBindTexture(GL_TEXTURE_2D, mapSymbols.getAtlasTexture());
    // тестирование
    // рисуем точки по которым идет текст
//        auto plainShader = shadersBucket.plainShader;
//        for (auto& drawTextItem: drawTextAlongPath) {
//            auto randomColor = drawTextItem.color;
//            for (auto point : drawTextItem.points) {
//                float data1[] = { FLOAT(point.x), FLOAT(-point.y) };
//                glUseProgram(plainShader->program);
//                glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, data1);
//                glEnableVertexAttribArray(plainShader->getPosLocation());
//                glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
//                glUniform1f(plainShader->getPointSizeLocation(), 40.0f);
//                glUniform4f(plainShader->getColorLocation(), randomColor[0], randomColor[1], randomColor[2], 1.0);
//                glDrawArrays(GL_POINTS, 0, 1);
//            }
//        }

    auto symbolShader = shadersBucket.symbolShader;
    glUseProgram(symbolShader->program);

    glUniformMatrix4fv(symbolShader->getProjectionMatrix(), 1, GL_FALSE, p.data());
    glUniform4f(symbolShader->getColorLocation(), 1.0, 0.0, 0.0, 1.0);
    glUniform1i(symbolShader->getTextureLocation(), 0);
    GLfloat red   = static_cast<GLfloat>(color.r) / 255;
    GLfloat green = static_cast<GLfloat>(color.g) / 255;
    GLfloat blue  = static_cast<GLfloat>(color.b) / 255;
    glUniform3f(symbolShader->getColorLocation(), red, green, blue);

    for (int drIndex = 0; drIndex < drawTextAlongPath.size(); drIndex++) {
        auto& drawTextItem = drawTextAlongPath[drIndex];

        std::vector<std::tuple<Symbol, float, float, float>> forRender {};
        float textWidth = 0;
        float textHeight = 0;
        float maxTop = 0;
        for (auto charSymbol : drawTextItem.wname) {
            Symbol symbol = mapSymbols.getSymbol(charSymbol);
            float w = symbol.width * symbolScale;
            float h = symbol.rows * symbolScale;
            float top = h - symbol.bitmapTop * symbolScale;
            if (top > maxTop) maxTop = top;

            float xPixelsShift = (symbol.advance >> 6) * symbolScale;
            textWidth += xPixelsShift;
            if (textHeight < h + top) textHeight = h + top;
            forRender.push_back({symbol, w, h, xPixelsShift});
        }

        auto sumLength = drawTextItem.legthOfPath;
        if (sumLength - 250 < textWidth) {
            // не поместится поэтому пропускаем
            continue;
        }

        auto points = drawTextItem.points;
        auto& latitude = drawTextItem.latitude;
        auto& longitude = drawTextItem.longitude;
        auto& currentZoom = mapNumbers.zoom;
        auto camLatitude = mapNumbers.camLatitude;
        auto camLongitudeNormalized = mapNumbers.camLongitudeNormalized;
        double tooFarDelta = (2.0 * M_PI) / pow(2, currentZoom);

        bool tooFarSkip = abs(camLatitude - latitude) > tooFarDelta || abs(camLongitudeNormalized - longitude) > tooFarDelta;
        if (tooFarSkip) {
            //continue;
        }

        //float startTextFrom = abs(sin(elapsedTime / 9)) * (sumLength - textWidth);
        float startTextFrom = sumLength / 2 - textWidth / 2;

        // calculate start point index
        int startPointIndex = 0;
        for (int i = 1; i < points.size(); i++) {
            auto& firstPoint = points[i - 1];
            auto& secondPoint = points[i];
            float distance = sqrt( pow(secondPoint.x - firstPoint.x, 2.0) + pow(secondPoint.y - firstPoint.y, 2.0) );
            if (distance <= startTextFrom) {
                startTextFrom -= distance;
                continue;
            }

            startPointIndex = i;
            break;
        }

        std::vector<DrawSymbol> drawSymbols = {};
        // start draw text
        int forRenderIndex = 0;
        for (int i = startPointIndex; i < points.size(); i++) {
            auto& firstPoint = points[i - 1];
            auto& secondPoint = points[i];
            float availableDistance = sqrt( pow(secondPoint.x - firstPoint.x, 2.0) + pow(secondPoint.y - firstPoint.y, 2.0) ) - startTextFrom;
            Eigen::Vector2f direction = Eigen::Vector2f(secondPoint.x - firstPoint.x, -secondPoint.y + firstPoint.y);
            direction.normalize();

            Eigen::Vector2f xVector(1.0f, 0.0f);
            float dotProduct = direction.dot(xVector);
            float angleRadians = std::acos(dotProduct);
            float shiftTextValue = textHeight / 2;
            float symbolDiff = startTextFrom; startTextFrom = 0;
            if (direction.x() > 0 && direction.y() < 0) {
                angleRadians *= -1;
            }

            while(availableDistance > 0 && forRenderIndex < forRender.size() && forRenderIndex >= 0) {
                Eigen::Vector2f diff = direction * symbolDiff;
                Eigen::Vector2f normalToDirection = Eigen::Vector2f(-direction.y(), direction.x());

                float pointX = firstPoint.x + diff.x();
                float pointY = -firstPoint.y + diff.y();
                auto charRender = forRender[forRenderIndex];
                Symbol symbol = std::get<0>(charRender);
                float w = std::get<1>(charRender);
                float h = std::get<2>(charRender);
                float pixelsShift = std::get<3>(charRender);

                float xPos = pointX;
                float yPos = pointY;

                Eigen::Vector2f translateBitmapLeft = symbol.bitmapLeft * symbolScale * normalToDirection;
                Eigen::Vector2f translateTopSymbol = (maxTop - (symbol.rows - symbol.bitmapTop)) * symbolScale * normalToDirection;
                Eigen::Vector2f shiftTextByNormal = normalToDirection * -shiftTextValue;

                Eigen::Matrix4f vmChar = vm *
                                         EigenGL::createTranslationMatrix(
                                                 translateBitmapLeft.x() + translateTopSymbol.x() + shiftTextByNormal.x(),
                                                 translateBitmapLeft.y() + translateTopSymbol.y() + shiftTextByNormal.y(),
                                                 0
                                         ) *
                                         EigenGL::createTranslationMatrix(xPos, yPos, 0) *
                                         EigenGL::createRotationMatrixZ(angleRadians) *
                                         EigenGL::createTranslationMatrix(-xPos, -yPos, 0);

                // Draw symbol
                std::vector<float> vertices = {
                        xPos, yPos,
                        xPos + w, yPos,
                        xPos + w, yPos + h,
                        xPos, yPos + h,
                };
                auto startU = symbol.startU(atlasW);
                auto endU = symbol.endU(atlasW);
                auto startV = symbol.startV(atlasH);
                auto endV = symbol.endV(atlasH);
                std::vector<float> textureCords = {
                        startU, startV,
                        endU, startV,
                        endU, endV,
                        startU, endV
                };
                drawSymbols.push_back(DrawSymbol { vertices, textureCords, vmChar });

                symbolDiff += pixelsShift;
                availableDistance -= pixelsShift;
                forRenderIndex++;
            }

            if (availableDistance < 0) {
                startTextFrom = -1 * availableDistance;
            }
        }

        for (auto& drawSymbol : drawSymbols) {
            glUniformMatrix4fv(symbolShader->getMatrixLocation(), 1, GL_FALSE, drawSymbol.vmChar.data());
            glVertexAttribPointer(symbolShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, drawSymbol.textureCords.data());
            glEnableVertexAttribArray(symbolShader->getTextureCord());
            glVertexAttribPointer(symbolShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, drawSymbol.vertices.data());
            glEnableVertexAttribArray(symbolShader->getPosLocation());
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
    }
}

void MapTileRender::renderTile(
        ShadersBucket &shadersBucket,
        MapTile *tile,
        MapCamera &mapCamera,
        Eigen::Matrix4f p,
        Eigen::Matrix4f vm,
        Eigen::Matrix4f pvm,
        float zoom,
        bool isForwardRendering,
        MapSymbols& mapSymbols,
        MapNumbers& mapNumbers,
        float elapsedTime,
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

    // Рисуем текст вдоль дорог
//    if (zoom > 15) {
//        renderPathText(tile, mapSymbols, vm, p, shadersBucket, mapNumbers, elapsedTime);
//    }
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
    float borderFactor = style.getBorderFactor(styleIndex);
    auto color = CommonUtils::toOpenGlColor(style.getColor(styleIndex));
    auto borderColor = CommonUtils::toOpenGlColor(style.getBorderColor(styleIndex));
    auto& polygons = tile->resultPolygonsAggregatedByStyles[styleIndex];
    auto& lines = tile->resultLinesAggregatedByStyles[styleIndex];
    auto visibleZoom = style.getVisibleZoom(styleIndex);
    auto forwardRenderingOnly = style.getForwardRenderingOnly(styleIndex);
    bool exitByFR = forwardRenderingOnly && !isForwardRendering;
    bool exByVisZoom = visibleZoom.find(floor(zoom)) == visibleZoom.end();
    if (exByVisZoom || exitByFR) {
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
    glUniform4fv(plainShader->getColorLocation(), 1, colorData);
    if (lineWidth != 0)  glLineWidth(lineWidth);

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
        glUniform4fv(roadShader->getColorLocation(), 1, colorData);
        glUniform4fv(roadShader->getBorderColorLocation(), 1, borderColor.data());
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
        glDrawElements(GL_TRIANGLES, wideLines.iboSize, GL_UNSIGNED_INT, 0);

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









