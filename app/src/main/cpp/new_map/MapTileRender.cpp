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
        Eigen::Matrix4f pv
) {
    drawBackground(shadersBucket, pv);
    for (auto styleIndex : style.getStyles()) {
        drawLayer(shadersBucket, tile, pv, styleIndex);
    }
}

MapStyle &MapTileRender::getStyle() {
    return style;
}

GLuint MapTileRender::getTilesTexture() {
    return tilesTexture;
}

void MapTileRender::renderTilesByLayers(ShadersBucket &shadersBucket, std::vector<TileAndMatrix> tiles) {
    auto& plainShader = shadersBucket.plainShader;
    for (auto& tileAndMatrix : tiles) {
        auto& matrix = tileAndMatrix.matrix;
        drawBackground(shadersBucket, matrix);
    }

    for (auto styleIndex : style.getStyles()) {
        for (auto& tileAndMatrix : tiles) {
            auto& matrix = tileAndMatrix.matrix;
            auto tile = tileAndMatrix.tile;
            drawLayer(shadersBucket, tile, matrix, styleIndex);
        }
    }
}

void MapTileRender::drawLayer(
        ShadersBucket& shadersBucket,
        MapTile* tile,
        Eigen::Matrix4f pv,
        int styleIndex
) {
    auto plainShader = shadersBucket.plainShader;
    float lineWidth = style.getLineWidth(styleIndex);
    auto color = CommonUtils::toOpenGlColor(style.getColor(styleIndex));
    auto& polygons = tile->resultPolygonsAggregatedByStyles[styleIndex];
    auto& lines = tile->resultLinesAggregatedByStyles[styleIndex];

    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
    glUniform4fv(plainShader->getColorLocation(), 1, color.data());
    if (!polygons.vertices.empty()) {
        glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT,
                              GL_FALSE, 0, polygons.vertices.data()
        );
        glEnableVertexAttribArray(plainShader->getPosLocation());
        glDrawElements(GL_TRIANGLES, polygons.indices.size(), GL_UNSIGNED_INT, polygons.indices.data());
    }

    if (!lines.vertices.empty()) {
        glLineWidth(lineWidth);
        glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT,
                              GL_FALSE, 0, lines.vertices.data()
        );
        glEnableVertexAttribArray(plainShader->getPosLocation());
        glDrawElements(GL_LINES, lines.indices.size(), GL_UNSIGNED_INT, lines.indices.data());
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
    auto backgroundColor = CommonUtils::toOpenGlColor(CSSColorParser::parse("rgb(241, 255, 230)"));
    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, matrix.data());
    glUniform4fv(plainShader->getColorLocation(), 1.0f, backgroundColor.data());
    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}









