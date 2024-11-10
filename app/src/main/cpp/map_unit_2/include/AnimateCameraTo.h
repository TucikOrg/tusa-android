//
// Created by Artem on 10.11.2024.
//

#ifndef TUSA_ANDROID_ANIMATECAMERATO_H
#define TUSA_ANDROID_ANIMATECAMERATO_H


#include "MapFpsCounter.h"
#include "MapNumbers.h"
#include <queue>

class AnimateCameraTo {
public:
    void animateTick(MapFpsCounter& mapFpsCounter, MapControls& mapControls) {
        if (animateToActive == false) {
            startNewAnimation(mapControls, mapFpsCounter);
            return;
        }

        float animationTime = mapFpsCounter.getTimeElapsed() - startTimeElapsed;
        float progress = animationTime / animationSeconds;
        float zoomDelta = targetZoom - startZoom;
        mapControls.setZoom(startZoom + zoomDelta * progress);

        float deltaLat = targetLatitude - startLatitude;
        float deltaLon = targetLongitude - startLongitude;
        mapControls.setCamPos(startLatitude + deltaLat * progress, startLongitude + deltaLon * progress);

        animateToActive = progress < 1.0;
    }

    void addAnimation(float zoom, float latitude, float longitude, float duration) {
        animations.push({zoom, latitude, longitude, duration});
    }

private:
    std::queue<std::array<float, 4>> animations;

    void startNewAnimation(MapControls& mapControls, MapFpsCounter& mapFpsCounter) {
        if(animations.empty()) {
            return;
        }

        LOGI("START NEW ANIMATION");
        auto animationData = animations.front();
        animations.pop();
        float zoom = animationData[0];
        float latitude = animationData[1];
        float longitude = animationData[2];
        float duration = animationData[3];

        startAnimation(zoom, latitude, longitude, duration, mapControls, mapFpsCounter);
    }

    void startAnimation(
            float zoom, float latitude, float longitude, float duration,
            MapControls& mapControls, MapFpsCounter& mapFpsCounter
    ) {
        animationSeconds = duration;
        startTimeElapsed = mapFpsCounter.getTimeElapsed();
        startZoom = mapControls.getZoom();
        startLatitude = mapControls.getCamLatitude();
        startLongitude = mapControls.getCamLongitude();

        targetZoom = zoom;
        targetLatitude = latitude;
        targetLongitude = longitude;
        animateToActive = true;
    }

    float animationSeconds = 0;
    float startTimeElapsed = 0;
    float startZoom = 0;
    float startLatitude = 0;
    float startLongitude = 0;

    float targetZoom = 0;
    float targetLatitude = 0;
    float targetLongitude = 0;
    bool animateToActive = false;
};


#endif //TUSA_ANDROID_ANIMATECAMERATO_H
