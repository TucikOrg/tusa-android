//
// Created by Artem on 30.09.2024.
//

#ifndef TUSA_ANDROID_MAPCONTROLS2_H
#define TUSA_ANDROID_MAPCONTROLS2_H

#include <cmath>
#include "util/android_log.h"

class MapControls2 {
public:
    MapControls2() {

    };

    void drag(float dx, float dy) {
        double scale = pow(2.0, std::fmin(scaleFactorZoom, maxZoom));
        double speed = moveSpeed / scale;
        epsg3857LatNorm -= dy * speed;
        epsg3857LatNorm = std::fmax(-1.0, std::fmin(1.0, epsg3857LatNorm));
        epsg3857LonNormInf += dx * speed;
    }

    void scale(float detectorScale) {
        float realScale = (detectorScale - 1.0f);
        scaleFactorRaw *= (1 + realScale * scaleSpeed);
        scaleFactorRaw = std::fmax(scaleShift, std::fmin(scaleFactorRaw, maxScale));
        float factor = scaleFactorRaw - scaleShift;
        scaleFactorZoom = factor;
    };

    void doubleTap() {

    }

    void setZoom(float zoom) {
        scaleFactorRaw = zoom + scaleShift;
        float factor = scaleFactorRaw - scaleShift;
        scaleFactorZoom = factor;
    }

    short getMaxTilesZoom() { return maxTilesZoom; }
    short getTilesZoom() { return getZoomFloor() > maxTilesZoom ? maxTilesZoom : getZoomFloor(); }
    short getZoomFloor() { return (short) scaleFactorZoom; }

    void setCamPos(float latitudeEPSG4326, float longitudeEPSG4326) {
        epsg3857LatNorm = Utils::EPSG4326_to_EPSG3857_latitude(latitudeEPSG4326) / M_PI;
        epsg3857LonNormInf = longitudeEPSG4326 / M_PI;
    }

    float getDistanceToMap(float portion) {
        float scale = pow(2.0, maxZoom - scaleFactorZoom);
        return camOneUnitScale * scale * portion;
    }

    double getEPSG3857LatitudeNorm() {
        return epsg3857LatNorm;
    }

    double getEPSG3857LongitudeNormInf() {
        return epsg3857LonNormInf;
    }

    double getEPSG3857Latitude() {
        return epsg3857LatNorm * M_PI;
    }

    double getEPSG4326Latitude() {
        return atan(sinh(getEPSG3857Latitude()));
    }

    float getVisPointDistortion() {
        return 1.0 / cos(getEPSG4326Latitude());
    }

    float getAnimatedTransition(float time) { return abs(cos(time * 0.1)); }

    float getCamDistDistortionImpact() {
        float from = 1.0f;
        float to = 3.0f;
        float secondFrom = 4.0f;
        float secondTo = 14.0f;
        float impact1 = 0.9;
        float impact2 = 0.75f;

        float zoom = getZoom();
        if (zoom <= from) {
            return 0.0f;
        }

        if (zoom > from && zoom < to) {
            float size = to - from;
            float p = (to - zoom) / size;
            p = std::fmax(0.0f, std::fmin(1.0f, p));
            return impact1 - p * impact1;
        }


        if (zoom <= secondFrom) {
            return impact1;
        }

        if (zoom > secondFrom && zoom < secondTo) {
            float size = secondTo - secondFrom;
            float p = 1.0 - (secondTo - zoom) / size;
            p = std::fmax(0.0f, std::fmin(1.0f, p));
            float delta = impact1 - impact2;
            return impact1 - p * delta;
        }

        return impact2;
    }

    float getTransition() {
        float transition = 1.0f;
        float from = 4.0f;
        float to = 5.0f;
        if (getZoom() > from && getZoom() < to) {
            transition = (to - getZoom()) / (to - from);
        }
        if (getZoom() >= to) {
            transition = 0;
        }
        return transition;
    }

    float getZoom() {
        return scaleFactorZoom;
    }

    void initCamUnit(float planeSize) {
        camOneUnitScale = planeSize / 500000.0f;
    }

    float transitionTest = 0.0;
private:
    int maxTilesZoom = 16;

    double epsg3857LatNorm = 0;
    double epsg3857LonNormInf = 0;

    float scaleFactorRaw = 0;
    float maxZoom = 19.0f;
    float scaleShift = 1.0f;
    float maxScale = maxZoom + scaleShift;
    float scaleSpeed = 0.5f;
    float scaleFactorZoom = 0;
    double moveSpeed = 0.001;
    float camOneUnitScale;
};


#endif //TUSA_ANDROID_MAPCONTROLS2_H
