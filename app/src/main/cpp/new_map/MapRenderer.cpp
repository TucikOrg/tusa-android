//
// Created by Artem on 16.09.2024.
//

#include "MapRenderer.h"
#include <GLES2/gl2.h>


void MapRenderer::renderFrame() {
    productionRender();

}

void MapRenderer::productionRender() {
    float deltaTime               = mapFpsCounter.newFrame();
    float timeElapsed             = mapFpsCounter.getTimeElapsed();

    mapControls.checkZoomUpdated();

    float shiftX                  = mapControls.getShiftX();
    short zTiles                  = mapControls.getTilesZoom();
    float distanceToMap           = mapControls.getDistanceToMap();
    float projectionNearFarDelta  = distanceToMap / 2;
    float planeZ                  = radius;
    float cameraDistance          = distanceToMap + radius;
    float cameraY                 = mapControls.getCameraY();
    float transition              = mapControls.getTransition();
    float cameraLatitudeRad       = mapControls.getCameraSphereLatitude(planeWidth);
    int lod                       = 20;
    GLuint mapTexture             = mapTileRender.getTilesTexture();
    int n                         = pow(2, zTiles);
    float nearPlane               = distanceToMap - projectionNearFarDelta;
    float farPlane                = distanceToMap + projectionNearFarDelta;
    float aspectRatio             = mapCamera.getRatio();

    Eigen::Matrix4f projection    = mapCamera.createPerspectiveProjection(nearPlane, farPlane);
    Eigen::Matrix4f viewToPlane   = mapCamera.createView(0, cameraY, cameraDistance, 0, cameraY, 0);
    Eigen::Matrix4f pvForPlane    = projection * viewToPlane;

    std::array<float, 2> visibleYEdges  = mapVisibleTiles.getYTilesVisibleEdges(zTiles, planeWidth, mapControls, pvForPlane);
    std::vector<int> visibleY           = mapVisibleTiles.getVisibleYTiles(visibleYEdges, zTiles);
    mapTileRender.renderMainTexture(shadersBucket, mapCamera, mapTileGetter, visibleY,  zTiles, shiftX, planeWidth, mapControls);




    std::vector<float> uv = {};
    std::vector<float> planetUV = {};
    std::vector<unsigned int> indices = {};
    std::vector<float> vertices = mapGeometry.generatePlainGeometry(uv, planetUV, indices, visibleY, zTiles, planeWidth, lod, planeZ);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    auto planet2Shader = shadersBucket.planet2Shader;
    glUseProgram(planet2Shader->program);
    glUniformMatrix4fv(planet2Shader->getProjectionMatrixLocation(), 1, GL_FALSE, projection.data());

    glVertexAttribPointer(planet2Shader->getPosLocation(), 3, GL_FLOAT,
                          GL_FALSE, 0, vertices.data()
    );
    glEnableVertexAttribArray(planet2Shader->getPosLocation());

    glVertexAttribPointer(planet2Shader->getPlanetUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, planetUV.data());
    glEnableVertexAttribArray(planet2Shader->getPlanetUVLocation());
    glVertexAttribPointer(planet2Shader->getTilesUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, uv.data());
    glEnableVertexAttribArray(planet2Shader->getTilesUVLocation());

    glBindTexture(GL_TEXTURE_2D, mapTexture);
    glUniform1i(planet2Shader->getTileTextureLocation(), 0);
    glUniform1f(planet2Shader->getTransitionLocation(), transition);
    glUniform1f(planet2Shader->getCamAngleLocation(), cameraLatitudeRad);
    glUniform1f(planet2Shader->getCamDistanceLocation(), cameraDistance);
    glUniform1f(planet2Shader->getRadiusLocation(), radius);
    glUniform1f(planet2Shader->getPlaneCamYLocation(), cameraY);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
    glDisable(GL_DEPTH_TEST);

    mapTest.drawPoints3D(shadersBucket, vertices, 5.0f, pvForPlane);
    mapTest.drawFPS(shadersBucket, mapSymbols, mapCamera, mapFpsCounter.getFps());
    float unitX = zTiles == 0 ? 1 : 2;
    float scale = 1;
    mapTest.drawTilesTextureTest(shadersBucket, mapCamera, mapTileRender.getTilesTexture(), unitX * scale, visibleY.size() * scale);
    auto error = CommonUtils::getGLErrorString();
}

void MapRenderer::onSurfaceChanged(int screenW, int screenH) {
    mapCamera = MapCamera(screenW, screenH, 60.0f);
    mapTest.init(mapCamera);
}

void MapRenderer::onSurfaceCreated(AAssetManager* assetManager) {
    shadersBucket.compileAllShaders(assetManager);
    mapSymbols.createFontTextures();
    mapTileRender.initTilesTexture();
}

void MapRenderer::init(AAssetManager *assetManager, JNIEnv *env, jobject& request_tile) {
    mapTileGetter = new MapTileGetter(env, request_tile, mapTileRender.getStyle());
    mapTileGetter->getOrRequest(0, 0, 0);
    mapSymbols.loadFont(assetManager);

}

void MapRenderer::drag(float dx, float dy) {
    mapControls.drag(dx, dy);
}

void MapRenderer::scale(float scaleFactor) {
    mapControls.scale(scaleFactor);
}

void MapRenderer::doubleTap() {
    mapControls.doubleTap();
}

MapRenderer::~MapRenderer() {
    delete mapTileGetter;
}

MapRenderer::MapRenderer() {
    mapControls.setShiftX(planeWidth / 2.0);
    mapControls.setCamYLimit(planeWidth / 2.0);
    mapControls.initStartZoom(0);
}




