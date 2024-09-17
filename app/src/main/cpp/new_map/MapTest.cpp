//
// Created by Artem on 16.09.2024.
//

#include "MapTest.h"
#include <GLES2/gl2.h>

void MapTest::drawPlainGeometryTest(
        ShadersBucket& shadersBucket,
        MapCamera& mapCamera,
        MapGeometry& mapGeometry
) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    std::shared_ptr<PlainShader> plainShader = shadersBucket.plainShader;
    glUseProgram(plainShader->program);

    auto projection = mapCamera.createPerspectiveProjection(0.1, 10.0f);
    auto view = mapCamera.createView(0, 0, 5, 0, 0, 0);
    Eigen::Matrix4f pv = projection * view;
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pv.data());

    const GLfloat color[] = { 1.0, 0.0, 0.0, 1.0};
    glUniform4fv(plainShader->getColorLocation(), 1, color);

    float width = 0.5f;
    float halfWidth = width / 2.0f;
    std::vector<float> uv = {};
    std::vector<unsigned int> indices = {};
    auto vertices = mapGeometry.generatePlainGeometry(width, 1, halfWidth, uv, indices);
    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT,
                          GL_FALSE, 0, vertices.data()
    );
    glEnableVertexAttribArray(plainShader->getPosLocation());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 2);
}


void MapTest::drawTileGeometryTest(
        ShadersBucket& shadersBucket,
        MapCamera& mapCamera,
        MapGeometry& mapGeometry,
        MapTile& mapTile,
        MapTileRender& mapTileRender
) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    float distance = 11000;
    auto projection = mapCamera.createPerspectiveProjection(distance, distance + 10);
    auto view = mapCamera.createView(2000, -2000, distance, 2000, -2000, 0);

    Eigen::Matrix4f pv = projection * view;
    std::vector<MapTile*> tiles = { &mapTile };
    mapTileRender.renderTiles(shadersBucket, tiles, pv);
}

void MapTest::drawRootTileGeometryTest(ShadersBucket &shadersBucket,
                                       MapCamera &mapCamera,
                                       MapGeometry &mapGeometry,
                                       MapTileGetter &mapTileGetter,
                                       MapTileRender &mapTileRender
                                       ) {
    auto& tile = mapTileGetter.getTile(0, 0, 0);
    if (tile.isEmpty())
        return;
    drawTileGeometryTest(shadersBucket, mapCamera, mapGeometry, tile, mapTileRender);
}



void MapTest::drawTilesToTextureTest(ShadersBucket &shadersBucket, MapCamera &mapCamera,
                                 MapGeometry &mapGeometry, MapTileGetter &mapTileGetter,
                                 MapTileRender &mapTileRender) {
    auto& tile = mapTileGetter.getTile(0, 0, 0);
    if (tile.isEmpty())
        return;

    float extent = 4096;
    auto projection = mapCamera.createOrthoProjection(0, extent, -extent, 0, 0.1, 1);
    auto view = mapCamera.createView();
    Eigen::Matrix4f pv = projection * view;
    mapTileRender.renderTilesToTexture(shadersBucket, mapCamera, { &tile }, pv);

    drawPlainTilesTextureTest(shadersBucket, mapCamera, mapGeometry, mapTileRender);
}

void MapTest::drawPlainTilesTextureTest(
        ShadersBucket& shadersBucket,
        MapCamera& mapCamera,
        MapGeometry& mapGeometry,
        MapTileRender& mapTileRender
) {
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    auto planet2Shader = shadersBucket.planet2Shader;
    glUseProgram(planet2Shader->program);

    auto projection = mapCamera.createPerspectiveProjection(0.1, 10.0f);
    auto view = mapCamera.createView(0, 0, 5, 0, 0, 0);
    Eigen::Matrix4f pv = projection * view;
    glUniformMatrix4fv(planet2Shader->getMatrixLocation(), 1, GL_FALSE, pv.data());

    float width = 2.0f;
    float radius = width / 2.0f;
    std::vector<float> uv = {};
    std::vector<unsigned int> indices = {};
    auto vertices = mapGeometry.generatePlainGeometry(width, 20, radius, uv, indices);
    glVertexAttribPointer(planet2Shader->getPosLocation(), 3, GL_FLOAT,
                          GL_FALSE, 0, vertices.data()
    );
    glEnableVertexAttribArray(planet2Shader->getPosLocation());

    glVertexAttribPointer(planet2Shader->getUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, uv.data());
    glEnableVertexAttribArray(planet2Shader->getUVLocation());

    glBindTexture(GL_TEXTURE_2D, mapTileRender.getTilesTexture());
    glUniform1i(planet2Shader->getTileTextureLocation(), 0);

    glUniform1f(planet2Shader->getAnimationLocation(), 0.0f);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

    glDisable(GL_DEPTH_TEST);
    //drawPoints2D(shadersBucket, vertices, 20.0f, pv);
}

void MapTest::drawPoints2D(ShadersBucket& shadersBucket, std::vector<float>& vertices, float pointSize, Eigen::Matrix4f& matrix) {
    auto plainShader = shadersBucket.plainShader;
    const GLfloat color[] = { 1, 0, 0, 1};
    glUseProgram(plainShader->program);
    glUniform1f(plainShader->getPointSizeLocation(), pointSize);
    glUniform4fv(plainShader->getColorLocation(), 1, color);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, matrix.data());
    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glDrawArrays(GL_POINTS, 0, vertices.size() / 2);
}
