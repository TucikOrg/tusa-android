//
// Created by Artem on 15.10.2024.
//

#ifndef TUSA_ANDROID_MAPENVIRONMENT_H
#define TUSA_ANDROID_MAPENVIRONMENT_H

#include <cmath>
#include <Eigen/Dense>
#include "MapColors.h"
#include "shader/shaders_bucket.h"

class MapEnvironment {
public:
    void init(float planeSize);

    float maxLat = atan(sinh(M_PI));
    void drawTopCap(
            Eigen::Matrix4f pv,
            Eigen::Matrix4f sphereModelMatrix,
            float zoom,
            float planeSize,
            ShadersBucket& shadersBucket
    );
    void drawBottomCap(
            Eigen::Matrix4f pv,
            Eigen::Matrix4f sphereModelMatrix,
            float zoom,
            float planeSize,
            ShadersBucket& shadersBucket
    );
private:
    void createTopCap(float planeSize);
    void createBottomCap(float planeSize);
    Eigen::Matrix4f translateSphere;
    GLuint topCapVbo;
    GLuint topCapIbo;
    unsigned int topCapIboSize;

    GLuint bottomCapVbo;
    GLuint bottomCapIbo;
    unsigned int bottomCapIboSize;
};


#endif //TUSA_ANDROID_MAPENVIRONMENT_H
