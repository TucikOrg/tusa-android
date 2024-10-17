//
// Created by Artem on 17.10.2024.
//

#ifndef TUSA_ANDROID_USERMARKER2_H
#define TUSA_ANDROID_USERMARKER2_H

#include <GLES2/gl2.h>
#include "shader/shaders_bucket.h"
#include "util/android_log.h"
#include <Eigen/Dense>

class UserMarker2 {
public:
    GLuint textureId;
    float latitude;
    float longitude;

    void draw(ShadersBucket& shadersBucket, Eigen::Matrix4f pvm);
};


#endif //TUSA_ANDROID_USERMARKER2_H
