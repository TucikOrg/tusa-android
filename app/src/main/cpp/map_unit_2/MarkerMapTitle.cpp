//
// Created by Artem on 11.11.2024.
//

#include "MarkerMapTitle.h"

void MarkerMapTitle::draw(ShadersBucket &shadersBucket,
                          Eigen::Matrix4f &pv,
                          MapNumbers &mapNumbers,
                          FromLatLonToSpherePos &fromLatLonToSpherePos,
                          MapSymbols& mapSymbols,
                          MapCamera& mapCamera
) {
    glDisable(GL_DEPTH_TEST);
    auto camLatitude = mapNumbers.camLatitude;
    auto camLongitudeNormalized = mapNumbers.camLongitudeNormalized;
    double noZTestDelta = M_PI / 2;
    bool visibleZoomSkip = visibleZoom.find(mapNumbers.tileZ) == visibleZoom.end();
    bool enableDepthTest = abs(camLatitude - latitude) > noZTestDelta || abs(camLongitudeNormalized - longitude) > noZTestDelta;
    if (enableDepthTest || visibleZoomSkip) {
        return;
    }

    Eigen::Vector3d position = fromLatLonToSpherePos.getPoint(mapNumbers, latitude, longitude);
    float markerX = position[0];
    float markerY = position[1];
    float markerZ = position[2];
    float scale = mapNumbers.scale * mapNumbers.distortionDistanceToMapPortion;
    CSSColorParser::Color color = CSSColorParser::parse("rgb(0, 0, 0)");
    mapSymbols.renderText3DByAtlas(wname, markerX, markerY, markerZ, 0.01 * scale, color, pv, shadersBucket);
}
