//
// Created by Artem on 16.09.2024.
//

#include "MapRenderer.h"
#include <GLES2/gl2.h>



void MapRenderer::renderFrame() {
    //mapTest.drawPlainGeometryTest(shadersBucket, mapCamera, mapGeometry);
    //mapTest.drawRootTileGeometryTest(shadersBucket, mapCamera, mapGeometry, *mapTileGetter, mapTileRender);

    //mapTest.drawTilesToTextureTest(shadersBucket, mapCamera, mapGeometry, *mapTileGetter, mapTileRender);

    productionRender();
}

void MapRenderer::productionRender() {
    auto deltaTime = mapFpsCounter.newFrame();
    auto timeElapsed = mapFpsCounter.getTimeElapsed();

    auto& tile = mapTileGetter->getTile(0, 0, 0);
    if (tile.isEmpty())
        return;

    float extent = 4096;
    auto projectionTileTexture = mapCamera.createOrthoProjection(0, extent, -extent, 0, 0.1, 1);
    auto viewTileTexture = mapCamera.createView();
    Eigen::Matrix4f pvTileTexture = projectionTileTexture * viewTileTexture;
    mapTileRender.renderTilesToTexture(shadersBucket, mapCamera, { &tile }, pvTileTexture);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    auto planet2Shader = shadersBucket.planet2Shader;
    glUseProgram(planet2Shader->program);

    auto projection = mapCamera.createPerspectiveProjection(0.1, 10.0f);
    auto camLatitude = mapControls.getCameraLatitudeRad();
    auto camPos = mapCamera.createSphericalCameraPosition(4, camLatitude);
    auto view = mapCamera.createView(camPos[0], camPos[1], camPos[2], 0, 0, 0);
    Eigen::Matrix4f pv = projection * view;
    glUniformMatrix4fv(planet2Shader->getMatrixLocation(), 1, GL_FALSE, pv.data());

    float width = 2.0f;
    float radius = width / 2.0f;
    std::vector<float> uv = {};
    std::vector<unsigned int> indices = {};
    auto vertices = mapGeometry.generatePlainGeometry(width, 50, radius, uv, indices);
    glVertexAttribPointer(planet2Shader->getPosLocation(), 3, GL_FLOAT,
                          GL_FALSE, 0, vertices.data()
    );
    glEnableVertexAttribArray(planet2Shader->getPosLocation());

    glVertexAttribPointer(planet2Shader->getUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, uv.data());
    glEnableVertexAttribArray(planet2Shader->getUVLocation());

    glBindTexture(GL_TEXTURE_2D, mapTileRender.getTilesTexture());
    glUniform1i(planet2Shader->getTileTextureLocation(), 0);
    glUniform1f(planet2Shader->getAnimationLocation(), 1.0);
    //glUniform1f(planet2Shader->getAnimationLocation(), abs(cos(timeElapsed)));
    glUniform1f(planet2Shader->getTextureUShiftLocation(), mapControls.geTextureUShift());
    //glUniform1f(planet2Shader->getTextureUShiftLocation(), timeElapsed * 0.5);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
    glDisable(GL_DEPTH_TEST);


}

void MapRenderer::onSurfaceChanged(int screenW, int screenH) {
    mapCamera = MapCamera(screenW, screenH, 60.0f);
}

void MapRenderer::onSurfaceCreated(AAssetManager* assetManager) {
    mapTileRender.initTilesTexture();
    shadersBucket.compileAllShaders(assetManager);
}

void MapRenderer::init(AAssetManager *assetManager, JNIEnv *env, jobject& request_tile) {
    mapTileGetter = new MapTileGetter(env, request_tile, mapTileRender.getStyle());
    mapTileGetter->networkTilesStack.push({0, 0, 0});
}

void MapRenderer::drag(float dx, float dy) {
    mapControls.drag(dx, dy);
}

void MapRenderer::scale(float scaleFactor) {

}

MapRenderer::~MapRenderer() {
    delete mapTileGetter;
}


