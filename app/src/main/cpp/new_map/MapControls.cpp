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

void MapControls::checkScale(int windowXSize) {
    if (savedWindowXSize == -1) {
        savedWindowXSize = windowXSize;
    }
    float previousWinXSize = savedWindowXSize;
    float prevTilesZoom = checkSavedTilesZoom;

    short newZoom = getTilesZoom();
    if (prevTilesZoom != newZoom) {
        float currentShiftX = shiftX;
        if (newZoom > prevTilesZoom) {
            float scale = pow(2, newZoom - prevTilesZoom);
            float useShift = (windowXSize / 2.0f);
            float windowChanged = abs(windowXSize - previousWinXSize);
            useShift += abs(windowChanged);
            float shiftValue = pow(2, newZoom) > windowXSize ? useShift : 0;
            float newShiftX = currentShiftX * scale + shiftValue;
            shiftX = newShiftX;
        } else if (newZoom < prevTilesZoom) {
            float scale = pow(2, prevTilesZoom - newZoom);
            float useShift = (previousWinXSize / 2.0f);
            float windowChanged = abs(windowXSize - previousWinXSize);
            useShift += abs(windowChanged);
            float shiftValue = pow(2, prevTilesZoom) > previousWinXSize ? useShift : 0;
            float newShiftX = (currentShiftX - shiftValue) / scale;
            shiftX = newShiftX;
        }
    }

    checkSavedTilesZoom = newZoom;
    savedWindowXSize = windowXSize;
}

void MapControls::scale(float detectorScale) {
    float realScale = (detectorScale - 1.0f);
    scaleFactorRaw *= (1 + realScale * scaleSpeed);
    scaleFactorRaw = std::fmax(scaleShift, std::fmin(scaleFactorRaw, maxScale));
    float factor = scaleFactorRaw - scaleShift;
    scaleFactorZoom = factor;
}

void MapControls::initStartZoom(float startZoom) {
    onAppStartMapZoom = startZoom;
    scaleFactorRaw = onAppStartMapZoom + scaleShift;
    float factor = scaleFactorRaw - scaleShift;
    scaleFactorZoom = factor;

    maxScale = maxZoom + scaleShift;
    checkSavedTilesZoom = getTilesZoom();
}

float MapControls::getDistanceToMap() {
    float scale = pow(2.0, maxZoom - scaleFactorZoom);
    return camOneUnitScale * scale;
}

float MapControls::getTransition() {
    float transition = 1.0f;
    float from = 3.0f;
    float to = 10.0f;
    if (getZoom() > from && getZoom() < to) {
        transition = (to - getZoom()) / (to - from);
    }
    if (getZoom() >= to) {
        transition = 0;
    }
    return transition;
}


