//
// Created by Artem on 16.09.2024.
//

#include "MapCamera.h"
#include "util/eigen_gl.h"

Eigen::Matrix4f MapCamera::createPerspectiveProjection(float near, float far) {
    Eigen::Matrix4f projectionMatrix = EigenGL::createPerspectiveProjectionMatrix(
            fovy,
            (float) screenW / (float) screenH,
            near,
            far
    );
    return std::move(projectionMatrix);
}

Eigen::Matrix4f MapCamera::createView(
        float camX, float camY, float camZ,
        float targetX, float targetY, float targetZ
) {
    Eigen::Vector3f cameraPosition = Eigen::Vector3f(camX, camY, camZ);
    Eigen::Vector3f target(targetX, targetY, targetZ);
    Eigen::Vector3f up(0.0f, 1.0f, 0.0f);
    Eigen::Matrix4f viewMatrix = EigenGL::createViewMatrix(cameraPosition, target, up);
    return std::move(viewMatrix);
}

Eigen::Matrix4f MapCamera::createOrthoProjection(
        float left,
        float right,
        float bottom,
        float top,
        float near,
        float far
) {
    return EigenGL::createOrthoMatrix(left, right, bottom, top, near, far);
}

MapCamera::MapCamera(int screenW, int screenH, float fovy): screenW(screenW), screenH(screenH), fovy(fovy) {
    glViewportDeviceSize();
}

void MapCamera::glViewportDeviceSize() {
    glViewport(0, 0, screenW, screenH);
}

std::array<float, 3> MapCamera::createSphericalCameraPosition(float distance, float cameraLatitudeRad) {
    float camWorldX = 0;
    float camWorldY = distance * sin(cameraLatitudeRad);
    float camWorldZ = distance * cos(cameraLatitudeRad);
    return {camWorldX, camWorldY, camWorldZ};
}
