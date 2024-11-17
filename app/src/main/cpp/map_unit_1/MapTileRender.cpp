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
                    mapSymbols
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
                    mapSymbols
            );
        }
    }

    glDisable(GL_SCISSOR_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mapCamera.glViewportDeviceSize();
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
        unsigned short from,
        unsigned short to
) {
    auto styles = tile->style.getStylesVec();
    auto stylesSet = tile->style.getStyles();
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
//        std::vector<DrawTextAlongPath> drawTextAlongPath = tile->resultDrawTextAlongPath;
//        for (auto& text: drawTextAlongPath) {
//            auto point = text.points[0];
//            auto color = CSSColorParser::parse("rgb(255, 0, 0)");
//            mapSymbols.renderText2D(text.wname, point.x, -point.y, 1.0f, color, pvm, shadersBucket);
//        }
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
//    if (!tile->resultWideLineAggregatedByStyles[styleIndex].vertices.empty() && drawWideLines) {
//        // рисуем точки границы дороги
//        auto& wideLines = tile->resultWideLineAggregatedByStyles[styleIndex];
//        glUseProgram(roadShader->program);
//        glUniform1f(roadShader->getWidthLocation(), wideLineWidth);
//        glUniform1f(roadShader->getBorderFactorLocation(), borderFactor);
//        glUniformMatrix4fv(roadShader->getMatrixLocation(), 1, GL_FALSE, vm.data());
//        glUniformMatrix4fv(roadShader->getProjectionLocation(), 1, GL_FALSE, p.data());
//        glUniform4fv(roadShader->getColorLocation(), 1, colorData);
//        glUniform4fv(roadShader->getBorderColorLocation(), 1, borderColor.data());
//        glVertexAttribPointer(roadShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, wideLines.vertices.data());
//        glEnableVertexAttribArray(roadShader->getPosLocation());
//        glVertexAttribPointer(roadShader->getPerpendicularsLocation(), 2, GL_FLOAT, GL_FALSE, 0, wideLines.perpendiculars.data());
//        glEnableVertexAttribArray(roadShader->getPerpendicularsLocation());
//        glVertexAttribPointer(roadShader->getUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, wideLines.uv.data());
//        glEnableVertexAttribArray(roadShader->getUVLocation());
//        glUniform1f(roadShader->getPointSizeLocation(), 20.0f);
//        glUniform4f(roadShader->getColorLocation(), 1.0f, 0.0f, 0.0f, 1.0f);
//        glDrawArrays(GL_POINTS, 0, wideLines.vertices.size() / 2.0);
//
//        glUseProgram(plainShader->program);
//        glUniform1f(plainShader->getPointSizeLocation(), 20.0f);
//        glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
//        glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, wideLines.vertices.data());
//        glEnableVertexAttribArray(plainShader->getPosLocation());
//        glUniform4f(plainShader->getColorLocation(), 1.0f, 0.0f, 0.0f, 1.0f);
//        glDrawArrays(GL_POINTS, 0, wideLines.vertices.size() / 2.0);
//    }

    // тестовые точки
//    glUseProgram(plainShader->program);
//    glUniform1f(plainShader->getPointSizeLocation(), 40.0f);
//    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
//    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, tile->lastPointsTest.data());
//    glEnableVertexAttribArray(plainShader->getPosLocation());
//    glUniform4f(plainShader->getColorLocation(), 0.0f, 1.0f, 0.0f, 1.0f);
//    glDrawArrays(GL_POINTS, 0, tile->lastPointsTest.size() / 2.0);
//
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








