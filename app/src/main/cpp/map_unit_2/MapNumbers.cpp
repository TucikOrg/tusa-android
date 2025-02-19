//
// Created by Artem on 15.10.2024.
//

#include "MapNumbers.h"

MapNumbers::MapNumbers(
        MapControls &mapControls,
        MapCamera &mapCamera,
        float planeSize,
        int textureTileSizeUnit,
        float forwardRenderingToWorldZoom
) {
    screenWidth = mapCamera.getScreenW();
    screenHeight = mapCamera.getScreenH();

    Eigen::Matrix4f projectionScreen = mapCamera.createOrthoProjection(0, screenWidth, screenHeight, 0, 0.1, 2);
    Eigen::Matrix4f viewScreen = mapCamera.createView();
    pvScreen = projectionScreen * viewScreen;

    this->mapControls = &mapControls;
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
    float impact = mapControls.getCamDistDistortionImpact();
    distortionDistanceToMapPortion = invDistortion * impact + (1.0 - impact); // вдияние дисторции на дистанцию до карты
    distanceToMap = mapControls.getDistanceToMap(distortionDistanceToMapPortion);
    maxDistanceToMap = mapControls.getMaxDistanceToMap();
    zoomingDelta = mapControls.getZoomingDelta();

    float planesDelta = distanceToMap / 1.1f;
    mapNearPlaneDelta = 1.0;
    float nearPlane = distanceToMap - mapNearPlaneDelta;
    float farPlane = distanceToMap + planeSize * 4.0;
    float extent = 4096;

    double shiftPlaneY = -1.0 * EPSG3857CamLatNorm * (planeSize * 0.5 / distortion);
    Eigen::Matrix4d scalePlane = EigenGL::createScaleMatrix(invDistortion, invDistortion, 1.0);
    Eigen::Matrix4d translatePlane = EigenGL::createTranslationMatrix(0.0, shiftPlaneY, 0.0);
    planeModelMatrix = translatePlane * scalePlane;
    sphereModelMatrix = EigenGL::createRotationMatrixAxis(EPSG4326CamLat, Eigen::Vector3d {1.0, 0.0, 0.0});

    projection = mapCamera.createPerspectiveProjectionD(nearPlane, farPlane);
    view = mapCamera.createViewD(0, 0, distanceToMap, 0, 0, 0, Eigen::Vector3d(0.0, 1.0, 0.0));
    pv = projection * view;

    pvFloat = pv.cast<float>();
    planeModelMatrixFloat = planeModelMatrix.cast<float>();
    sphereModelMatrixFloat = sphereModelMatrix.cast<float>();

    textureTileSize = textureTileSizeUnit;
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
}


