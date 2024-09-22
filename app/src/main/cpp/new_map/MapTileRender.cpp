//
// Created by Artem on 16.09.2024.
//

#include "MapTileRender.h"
#include "shader/shaders_bucket.h"
#include "util/android_log.h"
#include "util/eigen_gl.h"

void MapTileRender::initTilesTexture() {
    glGenFramebuffers(1, &tilesFrameBuffer);
    glGenFramebuffers(1, &tileFrameBuffer);

    glGenTextures(1, &tilesTexture);
    glBindTexture(GL_TEXTURE_2D, tilesTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void MapTileRender::renderMainTexture(
        ShadersBucket& shadersBucket,
        MapCamera& mapCamera,
        MapTileGetter* mapTileGetter,
        std::vector<int> xTiles,
        std::vector<int> yTiles,
        int tilesZoom,
        float shiftX
) {
    float windowXLen = tilesZoom == 0 ? 1 : 2;
    float windowYLen = yTiles.size();
    float tileTextureScale = tilesZoom == 0 ? 2 : 1;
    float extent = 4096;

    float tileTexSize = textureSizeForTile * tileTextureScale;
    float textureWidth = tileTexSize * windowXLen;
    float textureHeight = tileTexSize * windowYLen;

    glBindTexture(GL_TEXTURE_2D, tilesTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, tilesFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tilesTexture, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, textureWidth, textureHeight);
    glEnable(GL_SCISSOR_TEST);

    auto projectionTileTexture = mapCamera.createOrthoProjection(0, windowXLen * extent, -windowYLen * extent, 0, 0.1, 1);
    auto viewTileTexture = mapCamera.createView();
    Eigen::Matrix4f pv = projectionTileTexture * viewTileTexture;
    auto textureShader = shadersBucket.textureShader;

//    {
//        glDisable(GL_SCISSOR_TEST);
//        auto xTile = 0;
//        auto yTile = 0;
//        auto zTile = 0;
//        int yTileIndex = 0;
//        int zDelta = tilesZoom - zTile;
//        int scaleVisual2TopLeft = pow(2, zDelta);
//        float moveShiftX = fmod(shiftX, 1.0);
//        for (int xTileIndex = 0; xTileIndex < 2; xTileIndex++) {
//            float xi = xTileIndex - moveShiftX;
//            float yi = yTileIndex;
//            auto tile = mapTileGetter->getOrRequest(xTile, yTile, zTile);
//            if (tile->isEmpty())
//                continue;
//
//            float translateX = xi * extent * scaleVisual2TopLeft;
//            float translateY = -yi * extent * scaleVisual2TopLeft;
//            Eigen::Matrix4f translate = EigenGL::createTranslationMatrix(translateX, translateY, 0);
//            Eigen::Matrix4f scale = EigenGL::createScaleMatrix(scaleVisual2TopLeft, scaleVisual2TopLeft, 1);
//            Eigen::Matrix4f matrix = pv * translate * scale;
//            renderTile(shadersBucket, tile, mapCamera, matrix);
//        }
//    }


    float moveShiftX = fmod(shiftX, 1.0);
    for (int xTileIndex = 0; xTileIndex < xTiles.size(); xTileIndex++) {
        for (int yTileIndex = 0; yTileIndex < yTiles.size(); yTileIndex++) {
            auto xTile = xTiles[xTileIndex];
            auto yTile = yTiles[yTileIndex];
            float xi = xTileIndex - moveShiftX;
            float yi = yTileIndex;
            auto tile = mapTileGetter->getOrRequest(xTile, yTile, tilesZoom);
            if (tile->isEmpty())
                continue;

            float translateX = xi * extent;
            float translateY = -yi * extent;
            Eigen::Matrix4f translate = EigenGL::createTranslationMatrix(translateX, translateY, 0);
            Eigen::Matrix4f matrix = pv * translate;
            glScissor(xi * tileTexSize, ((yTiles.size() - 1) - yi) * tileTexSize, tileTexSize, tileTexSize);
            renderTile(shadersBucket, tile, mapCamera, matrix);
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
        Eigen::Matrix4f matrix
) {
    float extent = 4096;
    std::vector<float> vertices = {
            0, 0,
            0, -extent,
            extent, -extent,
            extent, 0,
    };
    auto& plainShader = shadersBucket.plainShader;
    auto backgroundColor = CommonUtils::toOpenGlColor(CSSColorParser::parse("rgb(241, 255, 230)"));
    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, matrix.data());
    glUniform4fv(plainShader->getColorLocation(), 1.0f, backgroundColor.data());
    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    for (auto styleIndex : style.getStyles()) {
        float lineWidth = style.getLineWidth(styleIndex);
        auto color = CommonUtils::toOpenGlColor(style.getColor(styleIndex));
        auto& polygons = tile->resultPolygonsAggregatedByStyles[styleIndex];
        auto& lines = tile->resultLinesAggregatedByStyles[styleIndex];

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
}


