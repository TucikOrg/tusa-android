//
// Created by Artem on 19.09.2024.
//

#include "MapVisibleTiles.h"
#include "util/android_log.h"
#include "util/eigen_gl.h"

std::array<std::array<float, 2>, 2> MapVisibleTiles::getPlaneCorners(
        Eigen::Matrix4f pv,
        float planeZ
) {
    std::vector<Eigen::Vector4f> planes = EigenGL::extractFrustumPlanes(pv);

    auto leftPlane = planes[0];
    auto rightPlane = planes[1];
    auto bottomPlane = planes[2];
    auto topPlane = planes[3];

    auto topLeftPoint = getPlanePoint(topPlane, leftPlane, planeZ);
    auto bottomRightPoint = getPlanePoint(rightPlane, bottomPlane, planeZ);

    return { topLeftPoint, bottomRightPoint };
}



std::array<float, 2> MapVisibleTiles::getPlanePoint(
        Eigen::Vector4f firstPlane,
        Eigen::Vector4f secondPlane,
        float planeZ
) {
    float A1 = firstPlane[0];
    float B1 = firstPlane[1];
    float C1 = firstPlane[2];
    float D1 = firstPlane[3];
    float d1 = C1 * planeZ + D1;

    float A2 = secondPlane[0];
    float B2 = secondPlane[1];
    float C2 = secondPlane[2];
    float D2 = secondPlane[3];
    float d2 = C2 * planeZ + D2;

    float det = A1 * B2 - A2 * B1;
    float d1D = (-d1) * B2 - (-d2) * B1;
    float d2D = A1 * (-d2) - A2 * (-d1);

    float x = d1D / det;
    float y = d2D / det;
    return {x, y};
}

std::array<float, 2> MapVisibleTiles::getYTilesVisibleEdges(int tilesZoom, float planeWidth, MapControls &mapControls, Eigen::Matrix4f pvForPlane) {
    float cameraY       = mapControls.getCameraY();
    float halfPlane     = planeWidth / 2.0;
    float pY            = cameraY / halfPlane;
    int n               = pow(2, tilesZoom);
    float tileSize      = planeWidth / n;

    Eigen::Matrix4f pvInverse = pvForPlane.inverse();
    Eigen::Vector4f ndcTopMiddle(0.0f, 1.0f, 0.5, 1.0f);
    Eigen::Vector4f topMiddlePoint = pvInverse * ndcTopMiddle;
    Eigen::Vector4f ndcBottomMiddle(0.0f, -1.0f, 0.5, 1.0f);
    Eigen::Vector4f bottomMiddlePoint = pvInverse * ndcBottomMiddle;
    topMiddlePoint /= topMiddlePoint.w();
    bottomMiddlePoint /= bottomMiddlePoint.w();

    float yTop = (topMiddlePoint.y() - halfPlane) / -planeWidth;
    float yBottom = (bottomMiddlePoint.y() - halfPlane) / -planeWidth;
    yTop = std::fmax(0.0, std::fmin(1.0, yTop));
    yBottom = std::fmax(0.0, std::fmin(1.0, yBottom));

    float yStart = yTop * n;
    float yEnd = yBottom * n;

    return { yStart, yEnd };
}
