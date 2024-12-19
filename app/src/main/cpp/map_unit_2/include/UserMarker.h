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

class UserMarker: public IGetPointData {
public:
    UserMarker() {}
    UserMarker(unsigned char* pixels, float latitude, float longitude, int64_t markerId, float startAnimationElapsedTime)
        : pixels(pixels), latitude(latitude),
          longitude(longitude), markerId(markerId),
          startAnimationElapsedTime(startAnimationElapsedTime) {
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

    bool uploadedToAtlas = false;
    int64_t markerId;
    unsigned char* pixels;
    AvatarAtlasPointer atlasPointer;
    float latitude;
    float longitude;
    float startAnimationElapsedTime = 0;
    float markerSize = 1.40f;
    float invertAnimationUnit = 0.0;
};


#endif //TUSA_ANDROID_USERMARKER_H
