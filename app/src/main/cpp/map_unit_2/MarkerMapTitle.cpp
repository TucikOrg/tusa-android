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
    auto& currentZoom = mapNumbers.zoom;
    glDisable(GL_DEPTH_TEST);
    auto camLatitude = mapNumbers.camLatitude;
    auto camLongitudeNormalized = mapNumbers.camLongitudeNormalized;
    double tooFarDelta = M_PI / pow(2, currentZoom);

    bool visibleZoomSkip = visibleZoom.find(mapNumbers.tileZ) == visibleZoom.end();
    bool tooFarSkip = abs(camLatitude - latitude) > tooFarDelta || abs(camLongitudeNormalized - longitude) > tooFarDelta;
    if (tooFarSkip || visibleZoomSkip) {
        return;
    }

    Eigen::Vector3d position = fromLatLonToSpherePos.getPoint(mapNumbers, latitude, longitude);
    float markerX = position[0];
    float markerY = position[1];
    float markerZ = position[2];
    float scale = mapNumbers.scale * mapNumbers.distortionDistanceToMapPortion;
    CSSColorParser::Color color = CSSColorParser::parse("rgb(0, 0, 0)");
    mapSymbols.renderText3DByAtlas(wname, markerX, markerY, markerZ, fontSize * scale, color, pv, shadersBucket);
}
