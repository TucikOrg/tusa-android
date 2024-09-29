//
// Created by Artem on 17.09.2024.
//

#include "MapControls.h"

#include "util/android_log.h"
#include <cmath>

MapControls::MapControls() {}

void MapControls::drag(float dx, float dy) {
    screenXDragged += dx;
    screenYDragged += dy;

    float moveScale = pow(2.0, scaleFactorZoom);
    cameraY += dy * (ySpeed / moveScale);
    cameraY = std::fmax(-camYLimit, std::fmin(cameraY, camYLimit));

    shiftX += dx * shiftXSpeed;
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
    short previousZoom = getTilesZoom();
    scaleFactorZoom = factor;
    short newZoom = getTilesZoom();
    int windowXSize = 2.0;
    if (previousZoom != newZoom) {
        float currentShiftX = shiftX;
        if (newZoom > previousZoom) {
            float scale = pow(2, newZoom - previousZoom);
            float shiftValue = pow(2, newZoom) > windowXSize;
            float newShiftX = currentShiftX * scale + shiftValue;
            shiftX = newShiftX;
        } else if (newZoom < previousZoom) {
            float scale = pow(2, previousZoom - newZoom);
            float shiftValue = pow(2, previousZoom) > windowXSize;
            float newShiftX = (currentShiftX - shiftValue) / scale;
            shiftX = newShiftX;
        }
    }
}

void MapControls::initStartZoom(float startZoom) {
    onAppStartMapZoom = startZoom;
    scaleFactorRaw = onAppStartMapZoom + scaleShift;
    float factor = scaleFactorRaw - scaleShift;
    scaleFactorZoom = factor;

    maxScale = maxZoom + scaleShift;
}

float MapControls::getDistanceToMap() {
    float scale = pow(2.0, maxZoom - scaleFactorZoom);
    return camOneUnitScale * scale;
}

float MapControls::getTransition() {
    float transition = 1.0f;
    float from = 4.0f;
    float to = 8.0f;
    if (getZoom() > from && getZoom() < to) {
        transition = (to - getZoom()) / from;
    }
    if (getZoom() >= to) {
        transition = 0;
    }
    return transition;
}


