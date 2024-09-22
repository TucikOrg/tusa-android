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

    float textureWidth = textureSizeForTile * windowXLen;
    float textureHeight = textureSizeForTile * windowYLen;
    glBindTexture(GL_TEXTURE_2D, tilesTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, tilesFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tilesTexture, 0);
    glViewport(0, 0, textureWidth, textureHeight);

    auto projectionTileTexture = mapCamera.createOrthoProjection(0, windowXLen, -windowYLen, 0, 0.1, 1);
    auto viewTileTexture = mapCamera.createView();
    Eigen::Matrix4f pv = projectionTileTexture * viewTileTexture;
    auto textureShader = shadersBucket.textureShader;
    std::vector<float> uv = {
            0, 1,
            0, 0,
            1, 0,
            1, 1
    };
    float signShiftX = shiftX < 0 ? -1 : 0;
    float moveShiftX = fmod(shiftX, 1.0);
    for (int xTileIndex = 0; xTileIndex < xTiles.size(); xTileIndex++) {
        for (int yTileIndex = 0; yTileIndex < yTiles.size(); yTileIndex++) {
            auto xTile = xTiles[xTileIndex];
            auto yTile = yTiles[yTileIndex];

            auto tile = mapTileGetter->getOrRequest(xTile, yTile, tilesZoom);
            if (tile->isEmpty())
                continue;

            float xi = xTileIndex - moveShiftX + signShiftX;
            float yi = yTileIndex;
            GLuint texture = getTileTexture(tile, shadersBucket, mapCamera);
            std::vector<float> vertices = {
                    xi, (float)-yi,
                    xi, (float)-yi - 1,
                    xi + 1, (float)-yi - 1,
                    xi + 1, (float)-yi,
            };

            glUseProgram(textureShader->program);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindTexture(GL_TEXTURE_2D, texture);
            glUniform1i(textureShader->getTileTextureLocation0(), 0);
            glUniformMatrix4fv(textureShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
            glVertexAttribPointer(textureShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, uv.data());
            glEnableVertexAttribArray(textureShader->getTextureCord());
            glVertexAttribPointer(textureShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
            glEnableVertexAttribArray(textureShader->getPosLocation());
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mapCamera.glViewportDeviceSize();
}

void MapTileRender::renderTile(
        ShadersBucket &shadersBucket,
        MapTile *tile,
        MapCamera& mapCamera
) {
    float extent = 4096;
    auto projectionTileTexture = mapCamera.createOrthoProjection(0, extent, -extent, 0, 0.1, 1);
    auto viewTileTexture = mapCamera.createView();
    Eigen::Matrix4f pv = projectionTileTexture * viewTileTexture;


    std::vector<float> vertices = {
            0, 0,
            0, -extent,
            extent, -extent,
            extent, 0,
    };
    auto& plainShader = shadersBucket.plainShader;
    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
    auto backgroundColor = CommonUtils::toOpenGlColor(CSSColorParser::parse("rgb(241, 255, 230)"));
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

GLuint MapTileRender::renderTileToTexture(
        ShadersBucket &shadersBucket,
        MapTile *tile,
        MapCamera &mapCamera
) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSizeForTile, textureSizeForTile, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, tileFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glViewport(0, 0, textureSizeForTile, textureSizeForTile);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    renderTile(shadersBucket, tile, mapCamera);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mapCamera.glViewportDeviceSize();
    return texture;
}

GLuint MapTileRender::getTileTexture(MapTile *tile, ShadersBucket& shadersBucket, MapCamera& mapCamera) {
    GLuint texture = 0;
    auto savedTexture = textures.find(tile->key());
    if (savedTexture == textures.end()) {
        texture = renderTileToTexture(shadersBucket, tile, mapCamera);
        textures[tile->key()] = texture;
    } else texture = savedTexture->second;
    return texture;
}
