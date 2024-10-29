//
// Created by Artem on 15.10.2024.
//

#ifndef TUSA_ANDROID_MAPENVIRONMENT_H
#define TUSA_ANDROID_MAPENVIRONMENT_H

#include <cmath>
#include <Eigen/Dense>
#include "MapColors.h"
#include "shader/shaders_bucket.h"
#include "MapNumbers.h"
#include <random>
#include <iostream>

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

    void drawStars(
            MapNumbers &mapNumbers,
            ShadersBucket &shadersBucket
    );

    void drawGlowing(
            MapNumbers &mapNumbers,
            ShadersBucket &shadersBucket
    );

    void draw(
            MapNumbers& mapNumbers,
            ShadersBucket& shadersBucket
    );

    void selectClearColor(float zoom);
private:
    void createTopCap(float planeSize);
    void createBottomCap(float planeSize);
    void createStars(float planeSize);
    Eigen::Matrix4f translateSphere;
    GLuint topCapVbo;
    GLuint topCapIbo;
    unsigned int topCapIboSize;

    GLuint bottomCapVbo;
    GLuint bottomCapIbo;
    unsigned int bottomCapIboSize;

    GLuint starsVbo;
    GLuint starsSizeVbo;
    unsigned int starsAmount;


    float drawSpaceZoomBorder = 5.0f;
    float glowSize = 0.0015f;
};


#endif //TUSA_ANDROID_MAPENVIRONMENT_H
