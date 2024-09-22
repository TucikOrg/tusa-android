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

    int n = pow(2, getTilesZoom());
    shiftX += dx * (shiftXSpeed);
    if (shiftX < 0) {
        shiftX += n;
    }
}

void MapControls::doubleTap() {
    if (transitionTest == 0.0) {
        transitionTest = 1.0;
    } else {
        transitionTest = 0.0;
    }
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
        // cдвиг по x нужно умножить или поделить на 2^(разница в зумах)
        // чтобы позиция камеры не менялась при измении зума
        // чтобы мы видели один и тот же участок карты только с разной детализацией
        int delta = abs(currentMapTilesZoom - newMapTilesZoom);
        int xFactor = pow(2, delta);
        if (currentMapTilesZoom < newMapTilesZoom) {
            shiftX = shiftX * xFactor + (newMapTilesZoom >= 2);
        } else {
            shiftX = (shiftX - (currentMapTilesZoom >= 2)) / xFactor;
        }
        currentMapTilesZoom = newMapTilesZoom;
    }
}

float MapControls::getShiftForZoom(int zoom) {
    int xFactor = pow(2, zoom);
    return shiftX * xFactor + (zoom >= 2);
}

