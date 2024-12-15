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
#include <cmath>

class UserMarker {
public:
    UserMarker() {}
    UserMarker(unsigned char* pixels, float latitude, float longitude, int64_t markerId);

    unsigned char* getPixels() { return pixels; }
    void setPosition(float latitude, float longitude);

    bool uploadedToAtlas = false;
    int64_t markerId;
    unsigned char* pixels;
    AvatarAtlasPointer atlasPointer;
    float latitude;
    float longitude;
    float startAnimationElapsedTime = 0;
    float markerSize = 1.40f;
    float invertAnimationUnit = 0.0;
private:

};


#endif //TUSA_ANDROID_USERMARKER_H
