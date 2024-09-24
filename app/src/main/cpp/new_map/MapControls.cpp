//
// Created by Artem on 17.09.2024.
//

#include "MapControls.h"

#include "util/android_log.h"
#include <cmath>

MapControls::MapControls() {

}

void MapControls::drag(float dx, float dy) {
    screenXDragged += dx;
    screenYDragged += dy;

    float moveScale = pow(2.0, scaleFactorZoom);
    cameraY += dy * (ySpeed / moveScale);
    cameraY = std::fmax(-camYLimit, std::fmin(cameraY, camYLimit));

    shiftX += dx * (shiftXSpeed / moveScale);
}

void MapControls::doubleTap() {
//    if (transitionTest == 0.0) {
//        transitionTest = 1.0;
//    } else {
//        transitionTest = 0.0;
//    }
    //test += 10;
    testBool = !testBool;
}

float MapControls::getCameraSphereLatitude(float planeWidth) {
    float p = cameraY / (planeWidth * 0.5);
    float lat = atan(sinh(p * M_PI));
    return lat;
}

void MapControls::scale(float detectorScale) {
    float realScale = (detectorScale - 1.0f);
    scaleFactorRaw *= (1 + realScale * scaleSpeed);
    scaleFactorRaw = std::fmax(scaleShift, std::fmin(scaleFactorRaw, maxScale));
    float factor = scaleFactorRaw - scaleShift;
    scaleFactorZoom = factor;

    newMapZoomFloor = getZoomFloor();
    newMapTilesZoom = getTilesZoom();
}

void MapControls::initStartZoom(float startZoom) {
    onAppStartMapZoom = startZoom;
    scaleFactorRaw = onAppStartMapZoom + scaleShift;
    float factor = scaleFactorRaw - scaleShift;
    scaleFactorZoom = factor;

    currentMapZoomFloor = getZoomFloor();
    newMapZoomFloor = currentMapZoomFloor;
    maxScale = maxZoom + scaleShift;
    currentMapTilesZoom = getTilesZoom();
    newMapTilesZoom = currentMapTilesZoom;
}

float MapControls::getDistanceToMap() {
    float scale = pow(2.0, maxZoom - scaleFactorZoom);
    return camOneUnitScale * scale;
}

float MapControls::getTransition() {
    return transitionTest;
    float transition = 1.0;
    float from = 4;
    float to = 5;
    if (getZoom() > from && getZoom() < to) {
        transition = to - getZoom();
    }
    if (getZoom() >= to) {
        transition = 0;
    }
    return 0.0;
    return transition;
}

void MapControls::checkZoomUpdated() {
    if (currentMapZoomFloor != newMapZoomFloor) {
        currentMapZoomFloor = newMapZoomFloor;
    }

    if (currentMapTilesZoom != newMapTilesZoom) {
        currentMapTilesZoom = newMapTilesZoom;
    }
}
