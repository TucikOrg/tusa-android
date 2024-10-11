//
// Created by Artem on 16.09.2024.
//

#include "MapTileRender.h"
#include "shader/shaders_bucket.h"
#include "util/android_log.h"
#include "util/eigen_gl.h"

void MapTileRender::initTilesTexture() {
    glGenFramebuffers(1, &tilesFrameBuffer);

    glGenTextures(1, &tilesTexture);
    glBindTexture(GL_TEXTURE_2D, tilesTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void MapTileRender::renderTile(
        ShadersBucket &shadersBucket,
        MapTile *tile,
        MapCamera &mapCamera,
        Eigen::Matrix4f p,
        Eigen::Matrix4f vm,
        Eigen::Matrix4f pvm,
        float zoom,
        bool isForwardRendering
) {
    drawBackground(shadersBucket, pvm);
    for (auto styleIndex : tile->style.getStyles()) {
        drawLayer(shadersBucket, tile, p, vm, pvm, styleIndex, zoom, isForwardRendering);
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

    if (!polygons.vertices.empty()) {
        glUseProgram(plainShader->program);
        glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
        glUniform4fv(plainShader->getColorLocation(), 1, colorData);
        glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT,
                              GL_FALSE, 0, polygons.vertices.data()
        );
        glEnableVertexAttribArray(plainShader->getPosLocation());
        glDrawElements(GL_TRIANGLES, polygons.indices.size(), GL_UNSIGNED_INT, polygons.indices.data());
    }

    bool drawWideLines = canDrawWideLines && isWideLine;
    if (!lines.vertices.empty() && !drawWideLines) {
        glUseProgram(plainShader->program);
        glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
        glUniform4fv(plainShader->getColorLocation(), 1, colorData);
        glLineWidth(lineWidth);
        glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT,
                              GL_FALSE, 0, lines.vertices.data()
        );
        glEnableVertexAttribArray(plainShader->getPosLocation());
        glDrawElements(GL_LINES, lines.indices.size(), GL_UNSIGNED_INT, lines.indices.data());
    }

    if (!tile->resultWideLineAggregatedByStyles[styleIndex].vertices.empty() && drawWideLines) {
        auto& wideLines = tile->resultWideLineAggregatedByStyles[styleIndex];
        glUseProgram(roadShader->program);
        glUniform1f(roadShader->getWidthLocation(), wideLineWidth);
        glUniform1f(roadShader->getBorderFactorLocation(), borderFactor);
        glUniformMatrix4fv(roadShader->getMatrixLocation(), 1, GL_FALSE, vm.data());
        glUniformMatrix4fv(roadShader->getProjectionLocation(), 1, GL_FALSE, p.data());
        glUniform4fv(roadShader->getColorLocation(), 1, colorData);
        glUniform4fv(roadShader->getBorderColorLocation(), 1, borderColor.data());
        glVertexAttribPointer(roadShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, wideLines.vertices.data());
        glEnableVertexAttribArray(roadShader->getPosLocation());
        glVertexAttribPointer(roadShader->getPerpendicularsLocation(), 2, GL_FLOAT, GL_FALSE, 0, wideLines.perpendiculars.data());
        glEnableVertexAttribArray(roadShader->getPerpendicularsLocation());
        glVertexAttribPointer(roadShader->getUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, wideLines.uv.data());
        glEnableVertexAttribArray(roadShader->getUVLocation());
        glDrawElements(GL_TRIANGLES, wideLines.indices.size(), GL_UNSIGNED_INT, wideLines.indices.data());
        glUniform4f(roadShader->getColorLocation(), 1.0f, 0.0f, 0.0f, 1.0f);
        glUniform1f(roadShader->getPointSizeLocation(), 20.0f);
        //glDrawArrays(GL_POINTS, 0, wideLines.vertices.size() / 2);
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
    if (!squarePoints.vertices.empty() && drawWideLines) {
        glUseProgram(squarePointsShader->program);
        glUniformMatrix4fv(squarePointsShader->getMatrixLocation(), 1, GL_FALSE, vm.data());
        glUniformMatrix4fv(squarePointsShader->getProjectionLocation(), 1, GL_FALSE, p.data());
        glUniform1f(squarePointsShader->getPointSizeLocation(), wideLineWidth - borderFactor * wideLineWidth * 2.0);
        glUniform4fv(squarePointsShader->getColorLocation(), 1, colorData);
        glVertexAttribPointer(squarePointsShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, squarePoints.vertices.data());
        glEnableVertexAttribArray(squarePointsShader->getPosLocation());
        glVertexAttribPointer(squarePointsShader->getUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, squarePoints.uvs.data());
        glEnableVertexAttribArray(squarePointsShader->getUVLocation());
        glVertexAttribPointer(squarePointsShader->getShiftLocation(), 2, GL_FLOAT, GL_FALSE, 0, squarePoints.shifts.data());
        glEnableVertexAttribArray(squarePointsShader->getShiftLocation());
        glDrawElements(GL_TRIANGLES, squarePoints.indices.size(), GL_UNSIGNED_INT, squarePoints.indices.data());
    }
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
    auto backgroundColor = CommonUtils::toOpenGlColor(CSSColorParser::parse("rgba(241, 255, 230, 1.0)"));
    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, matrix.data());
    glUniform4fv(plainShader->getColorLocation(), 1.0f, backgroundColor.data());
    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}









