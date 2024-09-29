//
// Created by Artem on 17.09.2024.
//

#ifndef TUSA_ANDROID_MAPCONTROLS_H
#define TUSA_ANDROID_MAPCONTROLS_H

#include "map/mercator.h"
#include "util/android_log.h"
#include <array>
#include <string>

class MapControls {
public:
    MapControls();

    void drag(float dx, float dy);
    void scale(float detector);
    void doubleTap();
    void initStartZoom(float startZoom);

    short getTilesZoom() { return getZoomFloor() > maxTilesZoom ? maxTilesZoom : getZoomFloor(); }
    short getZoomFloor() { return (short) scaleFactorZoom; }
    float getDistanceToMap();
    float getCameraSphereLatitude(float planeWidth);
    float getTransition();
    float getAnimatedTransition(float time) { return abs(cos(time * 0.1)); }

    void setCamYLimit(float limit) {
        camYLimit = limit;
    }

    float getZoom() {
        return scaleFactorZoom;
    }

    float getCameraY() {
        return cameraY;
    }

    float getShiftX() {
        return shiftX;
    }

    void checkScale(int windowXSize);

    float transitionTest = 0.0;
private:
    short checkSavedTilesZoom = 0;
    int savedWindowXSize = -1;

    int maxTilesZoom = 16;
    float screenXDragged = 0;
    float screenYDragged = 0;

    float cameraY = 0;
    float shiftX = 0;

    float onAppStartMapZoom = 0.0f;
    float scaleFactorRaw = 0;
    float maxZoom = 19.0f;
    float maxScale = 0;
    float scaleSpeed = 0.5f;
    float scaleShift = 1.0f;
    float scaleFactorZoom = 0;
    float ySpeed = -1500.0f;
    float shiftXSpeed = 0.0005f;
    float camOneUnitScale = 5.0f;
    float camYLimit = 0.0f;
};


#endif //TUSA_ANDROID_MAPCONTROLS_H
