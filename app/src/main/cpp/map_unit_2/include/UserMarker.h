//
// Created by Artem on 17.10.2024.
//

#ifndef TUSA_ANDROID_USERMARKER_H
#define TUSA_ANDROID_USERMARKER_H

#include <GLES2/gl2.h>
#include "shader/shaders_bucket.h"
#include "util/android_log.h"
#include <Eigen/Dense>
#include "util/eigen_gl.h"
#include "MapNumbers.h"
#include "FromLatLonToSpherePos.h"
#include "AvatarAtlasPointer.h"
#include "IGetPointData.h"
#include <cmath>
#include "avatars/Circle.h"

class UserMarker: public IGetPointData {
public:
    constexpr static const float defaultMarkerSize = 1.4;
    constexpr static const float minimumMarkerSize = 0.8f;

    UserMarker() {}
    UserMarker(unsigned char* pixels, float latitude, float longitude, int64_t markerId, float startAnimationElapsedTime)
        : pixels(pixels), latitude(latitude),
          longitude(longitude), markerId(markerId),
          startAnimationElapsedTime(startAnimationElapsedTime),
          startMovementAnimation(startAnimationElapsedTime),
          startMarkerSizeAnimationTime(startAnimationElapsedTime){
        cos_minus_lat = cos(-latitude);
        sin_minus_lat = sin(-latitude);
        sin_lon = sin(longitude);
        cos_lon = cos(longitude);
    }

    unsigned char* getPixels() { return pixels; }
    void setPosition(float latitude, float longitude) {
        this->latitude = latitude;
        this->longitude = longitude;
    }

    void newMovement(
            std::unordered_map<GLuint, void*>& refreshGroup,
            MapFpsCounter* mapFpsCounter,
            float newTargetX,
            float newTargetY,
            float movementAnimationTimeCONSTANT
    ) {
        if (newTargetX == movementTargetX && newTargetY == movementTargetY) {
            // если целевоt местоположение не изменилось то ничего не делаем
            return;
        }

        refreshGroup[atlasPointer.atlasId] = nullptr; // пересобрать буффера рендринга

        float progressUnClamp = (mapFpsCounter->getTimeElapsed() - startMovementAnimation) / movementAnimationTimeCONSTANT;
        float previousMovementProgress = std::clamp(progressUnClamp, 0.0f, 1.0f);
        Eigen::Vector2f startMovementPoint = Eigen::Vector2f {
                movementX,
                movementY
        };
        Eigen::Vector2f targetMovementPoint = Eigen::Vector2f {
                movementTargetX,
                movementTargetY
        };
        Eigen::Vector2f previousMovementDelta = targetMovementPoint - startMovementPoint;
        Eigen::Vector2f currentMovementPoint = startMovementPoint + previousMovementDelta * previousMovementProgress;

        movementX = currentMovementPoint.x();
        movementY = currentMovementPoint.y();

        movementTargetX = newTargetX;
        movementTargetY = newTargetY;

        startMovementAnimation = mapFpsCounter->getTimeElapsed();
    }

    void newMarkerSize(
            std::unordered_map<GLuint, void*>& refreshGroup,
            MapFpsCounter* mapFpsCounter,
            float newTargetMarkerSize,
            float markerSizeAnimationTimeCONSTANT
    ) {
        if (newTargetMarkerSize == targetMarkerSize) {
            // если целевой размер не изменился то ничего не делаем
            return;
        }

        refreshGroup[atlasPointer.atlasId] = nullptr; // пересобрать буффера рендринга

        float progressUnClamp = (mapFpsCounter->getTimeElapsed() - startMarkerSizeAnimationTime) / markerSizeAnimationTimeCONSTANT;
        float previousMarkerSizeProgress = std::clamp(progressUnClamp, 0.0f, 1.0f);
        float previousMarkerSizeDelta = targetMarkerSize - markerSize;
        float currentMarkerSize = markerSize + previousMarkerSizeDelta * previousMarkerSizeProgress;

        markerSize = currentMarkerSize;
        targetMarkerSize = newTargetMarkerSize;
        startMarkerSizeAnimationTime = mapFpsCounter->getTimeElapsed();
    }

    bool uploadedToAtlas = false;
    int64_t markerId;
    unsigned char* pixels;
    AvatarAtlasPointer atlasPointer;
    float latitude;
    float longitude;
    float startAnimationElapsedTime = 0;

    float invertAnimationUnit = 0.0;
    float movementX = 0.0f;
    float movementY = 0.0f;
    float movementTargetX = 0.0f;
    float movementTargetY = 0.0f;
    float startMovementAnimation = 0.0f;

    float markerSize = defaultMarkerSize;
    float startMarkerSizeAnimationTime = 0.0f;
    float targetMarkerSize = markerSize;
};


#endif //TUSA_ANDROID_USERMARKER_H
