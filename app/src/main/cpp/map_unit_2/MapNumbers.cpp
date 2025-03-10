//
// Created by Artem on 15.10.2024.
//

#include "MapNumbers.h"

MapNumbers::MapNumbers(
        MapControls &mapControls,
        MapCamera &mapCamera,
        float planeSize,
        int textureMapSize,
        float forwardRenderingToWorldZoom,
        float distortionDistanceToMapPortion,
        Eigen::Matrix4d projection
) {
    this->projection = projection;

    screenWidth = mapCamera.getScreenW();
    screenHeight = mapCamera.getScreenH();

    this->planeSize = planeSize;
    radius = planeSize / (2.0f * M_PI);

    EPSG3857LonNormInf = mapControls.getEPSG3857LongitudeNormInf();
    EPSG3857CamLatNorm = mapControls.getEPSG3857LatitudeNorm();
    EPSG3857CamLat = mapControls.getEPSG3857Latitude();
    EPSG4326CamLat = mapControls.getEPSG4326Latitude();
    distortion = mapControls.getVisPointDistortion();
    transition = mapControls.getTransition();
    double invDistortion = 1.0 / distortion;

    camLatitude = mapControls.getCamLatitude();
    camLongitude = mapControls.getCamLongitude();
    camLongitudeNormalized = mapControls.getCamLongitudeNormalized();

    tileZ = mapControls.getTilesZoom();
    zoom = mapControls.getZoom();


    if (zoom >= 2 && zoom <= 3.0) {
        float latMaxDelta = M_PI / 2.0;
        float zoom3Process = zoom - 2.0f;
        float moveDelta = latMaxDelta * (1.0 - zoom3Process + 0.15);
        if (abs(camLatitude) > latMaxDelta - moveDelta) {
            tileZ = 1;
        }
    }
    n = pow(2, tileZ);

    scale = mapControls.getScale();
    this->distortionDistanceToMapPortion = distortionDistanceToMapPortion;
    zoomingDelta = mapControls.getZoomingDelta();


    double shiftPlaneY = -1.0 * EPSG3857CamLatNorm * (planeSize * 0.5 / distortion);
    Eigen::Matrix4d translatePlane = EigenGL::createTranslationMatrix(0.0, shiftPlaneY, 0.0);
    scalePlane = EigenGL::createScaleMatrix(invDistortion, invDistortion, 1.0);

    planeModelMatrix = translatePlane * scalePlane;
    sphereModelMatrix = EigenGL::createRotationMatrixAxis(EPSG4326CamLat, Eigen::Vector3d {1.0, 0.0, 0.0});


    sphereModelMatrixFloat = sphereModelMatrix.cast<float>();

    textureTileSize = textureMapSize;
    visYTilesDelta = 1.0;
    visXTilesDelta = 1.0;
    if (n == 1) {
        visYTilesDelta = 0.5;
        visXTilesDelta = 0.5;
    }


    forwardRenderingToWorld = zoom >= forwardRenderingToWorldZoom;

    double camYNorm = (EPSG3857CamLatNorm - 1.0) / -2.0;
    double tileP = 1.0 / n;
    double camCenterYTile = camYNorm * n;
    camYStart = std::fmax(0.0, camCenterYTile - visYTilesDelta);
    camYEnd = std::fmin(n, camCenterYTile + visYTilesDelta);

    visTileYStart = floor(camYStart);
    visTileYEnd = ceil(camYEnd);
    int visTileYStartInv = n - visTileYEnd;
    int visTileYEndInv = n - visTileYStart;

    camXNorm = (EPSG3857LonNormInf + 1.0) / 2.0;
    yTilesAmount = visTileYEnd - visTileYStart;
    double camCenterXTile = camXNorm * n;
    camXStart = camCenterXTile - visXTilesDelta;
    double camXEnd = camCenterXTile + visXTilesDelta;
    visTileXStartInf = floor(camXStart);
    visTileXEndInf = ceil(camXEnd);
    xTilesAmount = abs(visTileXEndInf - visTileXStartInf);
    scaleUTex = visXTilesDelta * 2.0 / xTilesAmount;
    double tileUSize = (1.0 / xTilesAmount);
    tilesSwiped = (EPSG3857LonNormInf / (2.0 * tileP));
    double tilesUSwiped = tilesSwiped * tileUSize;
    EPSGLonNormInfNegative = (EPSG3857LonNormInf < 0);
    shiftUTex = fmod(tilesUSwiped, 1.0 / xTilesAmount) + EPSGLonNormInfNegative * tileUSize;

    planetVStart = visTileYStartInv * tileP;
    planetVEnd = visTileYEndInv * tileP;
    segments = zoom > 7 ? 1 : 60;
    planetVDelta = (planetVEnd - planetVStart) / segments;
    verticesShift = planeSize / 2.0;

    double planetUCenter = 0.5;
    double planetUVDelta = tileP * visXTilesDelta;
    planetUStart = std::fmax(planetUCenter - planetUVDelta, 0.0);
    planetUEnd = std::fmin(planetUCenter + planetUVDelta, 1.0);
    planetUDelta = (planetUEnd - planetUStart) / segments;

    double topPlanetV = planetVStart + segments * planetVDelta;
    double bottomPlanetV = planetVStart;
    double leftPlanetU = planetUStart;
    double rightPlanetU = planetUStart + segments * planetUDelta;

    topYVertex = topPlanetV * planeSize - verticesShift;
    leftXVertex = leftPlanetU * planeSize - verticesShift;
    bottomYVertex = bottomPlanetV * planeSize - verticesShift;
    rightXVertex = rightPlanetU * planeSize - verticesShift;

    leftX = Utils::normalizeXTile(visTileXStartInf, n);
    topY = visTileYStart;
}


