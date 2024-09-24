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
        std::vector<int> yTiles,
        int tilesZoom,
        float shiftX,
        float planeWidth,
        MapControls& mapControls
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
    auto textureShader = shadersBucket.textureShader;

    int n                           = pow(2, tilesZoom);
    float tileSize                  = planeWidth / n;
    float shiftCameraCenterInTiles  = shiftX / tileSize; // сдвиг камеры в тайлах
    float viewXInTilesDelta         = windowXLen / 2;
    float startShiftCameraInTiles   = shiftCameraCenterInTiles - viewXInTilesDelta;
    float endShiftCameraInTiles     = shiftCameraCenterInTiles + viewXInTilesDelta;

    Eigen::Matrix4f projectionTileTexture        = mapCamera.createOrthoProjection(0, windowXLen * extent, -windowYLen * extent, 0, 0.1, 1);
    float cameraX                                = shiftCameraCenterInTiles * extent - windowXLen * (extent / 2);
    Eigen::Matrix4f viewTileTexture              = mapCamera.createView(cameraX, 0, 1, cameraX, 0, 0);
    Eigen::Matrix4f pv                           = projectionTileTexture * viewTileTexture;

    {
        std::vector<int> backgroundXTiles = { 2 };
        std::vector<int> backgroundYTiles = { 2 };
        int backgroundZTiles            = 2;
        int bgN                         = pow(2, backgroundZTiles);
        int bgTileSize                  = planeWidth / bgN;
        float shiftBgCameraInTiles      = shiftX / bgTileSize;
        float shiftBgInTile             = fmod(shiftBgCameraInTiles, 1);
        int xTilesSize                  = backgroundXTiles.size();
        float zDelta                    = tilesZoom - backgroundZTiles;
        float portionOfBackgroundTile   = pow(2, zDelta);
        float bgTileTexSize             = tileTexSize * portionOfBackgroundTile;

        int yActualTop = yTiles[0];
        int yBackgroundTop = backgroundYTiles[0];
        float yDelta = (yBackgroundTop * portionOfBackgroundTile) - yActualTop;

        float bgLeftInTiles = shiftBgCameraInTiles - bgN / 2.0f;
        float bgLeftTileShift = fmod(bgLeftInTiles, bgN);
        int mapSwipedAmount   = (bgLeftInTiles) / bgN;

        int xBackgroundLeft = backgroundXTiles[0];
        float xBgDelta = xBackgroundLeft - fmod(fmod(startShiftCameraInTiles, n) / portionOfBackgroundTile, bgN);
        //xBgDelta = fmod(xBgDelta, bgN * 2);
        //LOGI("xBgDelta %f z %d", xBgDelta, tilesZoom);

        float bgOneTileWidth = extent * portionOfBackgroundTile;
        float bgMapWidth = bgOneTileWidth * bgN;

        for (int i = -1; i <= 1; i++) {
            for (float xTileIndex = 0; xTileIndex < xTilesSize; xTileIndex++) {
                int xTile = backgroundXTiles[xTileIndex];
                float translateX = xTile * bgOneTileWidth + (i + mapSwipedAmount) * bgMapWidth;

                for (int yTileIndex = 0; yTileIndex < backgroundYTiles.size(); yTileIndex++) {
                    int yTile = backgroundYTiles[yTileIndex];
                    auto tile = mapTileGetter->getOrRequest(xTile, yTile, backgroundZTiles);
                    if (tile->isEmpty())
                        continue;

                    float yTileShift = yTileIndex * portionOfBackgroundTile + yDelta;
                    float translateY = -yTileShift * extent;
                    Eigen::Matrix4f translate = EigenGL::createTranslationMatrix(translateX, translateY, 0);
                    Eigen::Matrix4f scale = EigenGL::createScaleMatrix(portionOfBackgroundTile, portionOfBackgroundTile, 1);
                    Eigen::Matrix4f matrix = pv * translate * scale;

                    //float xScissor = (xTileIndex + i * bgN - bgLeftTileShift) * bgTileTexSize;
                    float xScissor = (xTileIndex + i * bgN + xBgDelta) * bgTileTexSize;
                    float yScissor = (windowYLen - yDelta - (yTileIndex + 1) * portionOfBackgroundTile) * tileTexSize;

                    if (xScissor + bgTileTexSize > 0 && xScissor < textureWidth) {
                        glScissor(xScissor, yScissor, bgTileTexSize, bgTileTexSize);
                        renderTile(shadersBucket, tile, mapCamera, matrix);
                    }

                    if (i == 0) {
                        LOGI("xScissor %f yScissor %f bgTileTexSize %f", xScissor, yScissor, bgTileTexSize);
                    }
                }
            }
        }

    }


//    int xTileIndex = 0;
//    for (float camMercXCursor = startShiftCameraInTiles; Utils::round(camMercXCursor, 3) <= Utils::round(endShiftCameraInTiles, 3); camMercXCursor++, xTileIndex++) {
//        float xTile = fmod(camMercXCursor, n);
//        if (xTile < 0)
//            xTile += n;
//
//        for (int yTileIndex = 0; yTileIndex < yTiles.size(); yTileIndex++) {
//            auto yTile = yTiles[yTileIndex];
//            auto tile = mapTileGetter->getOrRequest(xTile, yTile, tilesZoom);
//            if (tile->isEmpty())
//                continue;
//
//            float xTranslateByCamXCursor = camMercXCursor;
//            if (xTranslateByCamXCursor < 0) {
//                xTranslateByCamXCursor = -ceil(abs(xTranslateByCamXCursor));
//            }
//            float translateX = (int) xTranslateByCamXCursor * extent;
//            float translateY = -yTileIndex * extent;
//            Eigen::Matrix4f translate = EigenGL::createTranslationMatrix(translateX, translateY, 0);
//            Eigen::Matrix4f matrix = pv * translate;
//            float xScissor = (-fmod(xTile, 1.0) + xTileIndex) * tileTexSize;
//            float yScissor = ((yTiles.size() - 1) - yTileIndex) * tileTexSize;
//            glScissor(xScissor, yScissor, tileTexSize, tileTexSize);
//            renderTile(shadersBucket, tile, mapCamera, matrix);
//        }
//    }

    glDisable(GL_SCISSOR_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mapCamera.glViewportDeviceSize();
}

void MapTileRender::renderTile(
        ShadersBucket &shadersBucket,
        MapTile *tile,
        MapCamera &mapCamera,
        Eigen::Matrix4f pv
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
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
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


