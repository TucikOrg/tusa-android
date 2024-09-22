
//
// Created by Artem on 16.09.2024.
//

#ifndef TUSA_ANDROID_MAPCAMERA_H
#define TUSA_ANDROID_MAPCAMERA_H

#include <GLES2/gl2.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/StdVector>
#include <array>

class MapCamera {
public:
    MapCamera(int screenW, int screenH, float fovy) ;
    MapCamera() : screenW(0), screenH(0), fovy(0) {}

    Eigen::Matrix4f createOrthoProjection(float left, float right, float bottom, float top, float near, float far);
    Eigen::Matrix4f createPerspectiveProjection(float near = 0.1, float far = 100.0f);
    Eigen::Matrix4f createView(float camX = 0, float camY = 0, float camZ = 1, float targetX = 0, float targetY = 0, float targetZ = 0);
    std::array<float, 3> createSphericalCameraPosition(float distance, float cameraLatitudeRad);

    void glViewportDeviceSize();
    float getRatio();

private:
    int screenW;
    int screenH;
    float fovy;
};


#endif //TUSA_ANDROID_MAPCAMERA_H
