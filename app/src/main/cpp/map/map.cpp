//
// Created by Artem on 01.01.2024.
//

#include "map/map.h"

#include <utility>
#include "renderer/renderer.h"
#include "network/get_tile_request.h"
#include "util/frustrums.h"


void Map::onSurfaceCreated(AAssetManager *assetManager) {
    shadersBucket->compileAllShaders(assetManager);

    renderer.onSurfaceCreated(assetManager);
}

void Map::onSurfaceChanged(int w, int h) {
    renderer.onSurfaceChanged(w, h);
}

void Map::render() {
    renderer.renderFrame();
}

void Map::noOpenGlContextInit(AAssetManager *assetManager, float scaleFactor, JNIEnv *env) {
    renderer.setupNoOpenGLMapState(scaleFactor, assetManager, env);
}

Map::Map(Cache *cache): cache(cache) { }

void Map::drag(float dx, float dy) {
    renderer.drag(dx, dy);
}

void Map::scale(float scaleFactor) {
    renderer.scale(scaleFactor);
}

void Map::doubleTap() {
    renderer.doubleTap();
}

void Map::onStop() {
    renderer.onStop();
}

void Map::onDown() {
    renderer.onDown();
}

void Map::addMarker(std::string key, float latitude, float longitude, unsigned char *imageData, off_t fileSize) {
    renderer.addMarker(std::move(key), latitude, longitude, imageData, fileSize);
}

void Map::handleMarker(std::string key, float latitude, float longitude, unsigned char *imageData, off_t fileSize) {
    renderer.handleMarker(std::move(key), latitude, longitude, imageData, fileSize);
}

void Map::updateMarkerAvatar(std::string key, unsigned char *imageData, off_t fileSize) {
    renderer.updateMarkerAvatar(std::move(key), imageData, fileSize);
}

void Map::updateMarkerGeo(std::string key, float latitude, float longitude) {
    renderer.updateMarkerGeo(std::move(key), latitude, longitude);
}

void Map::removeMarker(std::string key) {
    renderer.removeMarker(std::move(key));
}

