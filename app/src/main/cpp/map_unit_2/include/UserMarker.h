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
#include <cmath>

class UserMarker {
public:
    UserMarker() {}
    UserMarker(GLuint textureId, float latitude, float longitude);

    void draw(
            ShadersBucket &shadersBucket,
            Eigen::Matrix4f &pv,
            MapNumbers& mapNumbers,
            FromLatLonToSpherePos& fromLatLonToSpherePos
    );
    void drawDebugData(
            ShadersBucket &shadersBucket,
            Eigen::Matrix4f pv,
            MapNumbers &mapNumbers,
            float markerX,
            float markerY,
            float markerZ,
            float radius,
            Eigen::Vector3d axisLatitude,
            Eigen::Vector3d axisLongitude
    );

    void setTexture(GLuint textureId);
    void clearTexture();
    void setPosition(float latitude, float longitude);
private:
    GLuint textureId;
    float latitude;
    float longitude;
    float size = 2.5f;
};


#endif //TUSA_ANDROID_USERMARKER_H
