//
// Created by Artem on 15.12.2024.
//

#ifndef TUSA_ANDROID_AVATARSGROUP_H
#define TUSA_ANDROID_AVATARSGROUP_H

#include <GLES2/gl2.h>
#include <UserMarker.h>
#include <vector>

class AvatarsGroup {
public:
    GLuint atlasTexture;
    std::vector<UserMarker*> userMarkers;
    GLuint avatarsVBO;
    GLuint avatarsIBO;

    std::vector<float> avatarsTargetMarkerSize;
    std::vector<float> movementStartAnimation;
    std::vector<float> movementTargetLocation;
    std::vector<float> movementLocation;
    std::vector<float> invertAnimationUnit;
    std::vector<float> startAnimationElapsedTime;
    std::vector<float> latitudeLongitude;
    std::vector<float> startMarkerSizeAnimation;


    size_t avatarsIboSize = 0;
};


#endif //TUSA_ANDROID_AVATARSGROUP_H
