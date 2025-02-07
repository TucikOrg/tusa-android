//
// Created by Artem on 30.09.2024.
//

#ifndef TUSA_ANDROID_MAPCONTROLS_H
#define TUSA_ANDROID_MAPCONTROLS_H

#include <cmath>
#include <thread>
#include "util/android_log.h"
#include "MapFpsCounter.h"
#include "CameraPos.h"

class MapControls {
public:
    MapControls() {};

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
        zoomingDelta = factor - zoomingStart;
        scaleFactorZoom = factor;
    };
    void doubleTap() {}

    void allTilesReady() {
        zoomingStart = scaleFactorZoom;
    }

    float getCamLatitude() {
        return getEPSG4326Latitude();
    }

    float getCamLongitude() {
        return fmod(epsg3857LonNormInf * M_PI + M_PI, 2 * M_PI) - M_PI;
    }

    float getCamLongitudeNormalized() {
        float longitude = getCamLongitude();
        if (longitude >= M_PI) {
            longitude -= 2 * M_PI;
        }
        if (longitude <= -M_PI) {
            longitude += 2 * M_PI;
        }
        return longitude;
    }

    short getMaxTilesZoom() { return maxTilesZoom; }
    short getTilesZoom() { return getZoomFloor() > maxTilesZoom ? maxTilesZoom : getZoomFloor(); }
    short getZoomFloor() { return (short) scaleFactorZoom; }
    float getZoomingDelta() { return zoomingDelta; }
    float getScale() { return pow(2.0, maxZoom - scaleFactorZoom); }

    void setZoom(float zoom) {
        scaleFactorRaw = zoom + scaleShift;
        float factor = scaleFactorRaw - scaleShift;
        scaleFactorZoom = factor;
        zoomingStart = factor;
    }

    void setCamPos(float latitudeEPSG4326, float longitudeEPSG4326) {
        epsg3857LatNorm = Utils::EPSG4326_to_EPSG3857_latitude(latitudeEPSG4326) / M_PI;
        epsg3857LonNormInf = longitudeEPSG4326 / M_PI;
    }

    CameraPos getCamPos() {
        return CameraPos {
                getCamLatitude(),
                getCamLongitude(),
                getZoom()
        };
    }

    float getDistanceToMap(float portion) {
        float scale = getScale();
        return camOneUnitScale * scale * portion;
    }

    float getMaxDistanceToMap() {
        float scale = pow(2.0, maxZoom);
        return camOneUnitScale * scale;
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
        return getDistortion(getEPSG4326Latitude());
    }

    float getDistortion(float epsg4326Latitude) {
        return 1.0 / cos(epsg4326Latitude);
    }

    float getAnimatedTransition(float time) { return abs(cos(time * 0.1)); }

    float getCamDistDistortionImpact() {
        return 0.0f;

        float impact1 = 1.0f;
        float from = 3.0f;
        float to = 4.0f;

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

        return impact1;
    }

    float getTransition() {
        float transition = 1.0f;
        float from = 6.5f;
        float to = 7.0f;
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

    float getMaxZoom() {
        return maxZoom;
    }

    void initCamUnit(float planeSize) {
        camOneUnitScale = planeSize / 400000.0f;
    }

private:


    float zoomingDelta = 0.0;
    float zoomingStart = 0.0;
    int maxTilesZoom = 16;

    double epsg3857LatNorm = 0;
    double epsg3857LonNormInf = 0;

    float scaleFactorRaw = 0;
    float maxZoom = 19.0f;
    float scaleShift = 1.0f;
    float maxScale = maxZoom + scaleShift;
    float scaleSpeed = 0.4f;
    float scaleFactorZoom = 0;
    double moveSpeed = 0.001;
    float camOneUnitScale;
};


#endif //TUSA_ANDROID_MAPCONTROLS_H
