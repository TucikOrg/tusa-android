//
// Created by Artem on 16.09.2024.
//

#include "MapTileRender.h"
#include "shader/shaders_bucket.h"
#include "util/android_log.h"

void MapTileRender::renderTiles(
        ShadersBucket& shadersBucket,
        std::vector<MapTile*> tiles,
        Eigen::Matrix4f& pv
) {
    auto& plainShader = shadersBucket.plainShader;
    glUseProgram(plainShader->program);

    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pv.data());

    // Рисуем бекграунд
    auto backgroundColor = CommonUtils::toOpenGlColor(CSSColorParser::parse("rgb(241, 255, 230)"));
    glUniform4fv(plainShader->getColorLocation(), 1, backgroundColor.data());

    float extent = 4096;
    float backPoints[] = {
            0, 0,
            extent, 0,
            extent, -extent,
            0, -extent,
    };

    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT,
                          GL_FALSE, 0, backPoints
    );
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


    for (auto styleIndex : style.getStyles()) {
        float lineWidth = style.getLineWidth(styleIndex);
        auto color = CommonUtils::toOpenGlColor(style.getColor(styleIndex));
        glUniform4fv(plainShader->getColorLocation(), 1, color.data());

        for (auto tile : tiles) {
            auto& polygons = tile->resultPolygonsAggregatedByStyles[styleIndex];
            auto& lines = tile->resultLinesAggregatedByStyles[styleIndex];

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
}

void MapTileRender::renderTilesToTexture(
        ShadersBucket &shadersBucket,
        MapCamera &mapCamera,
        std::vector<MapTile *> tiles,
        Eigen::Matrix4f &pv
) {
    glBindTexture(GL_TEXTURE_2D, tilesTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, tilesFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tilesTexture, 0);
    glViewport(0, 0, tilesTextureWidth, tilesTextureHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    renderTiles(shadersBucket, tiles, pv);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mapCamera.glViewportDeviceSize();
}

void MapTileRender::initTilesTexture() {
    glGenFramebuffers(1, &tilesFrameBuffer);
    glGenTextures(1, &tilesTexture);

    glBindTexture(GL_TEXTURE_2D, tilesTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tilesTextureWidth, tilesTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}
