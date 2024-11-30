//
// Created by Artem on 11.11.2024.
//

#ifndef TUSA_ANDROID_MARKERMAPTITLE_H
#define TUSA_ANDROID_MARKERMAPTITLE_H

#include <string>
#include "shader/shaders_bucket.h"
#include "MapNumbers.h"
#include "FromLatLonToSpherePos.h"
#include "MapSymbols.h"
#include <Eigen/Dense>

class MarkerMapTitle {
public:
    MarkerMapTitle() {

    }
    MarkerMapTitle(
        std::wstring wname,
        float latitude,
        float longitude,
        float fontSize,
        std::unordered_map<short, void*> visibleZoom,
        uint64_t featureId,
        uint64_t placeLabelId
    ):
        wname(wname),
        latitude(latitude),
        longitude(longitude),
        fontSize(fontSize),
        visibleZoom(visibleZoom),
        featureId(featureId),
        placeLabelId(placeLabelId) {
    }

    void draw(
            ShadersBucket &shadersBucket,
            Eigen::Matrix4f &pv,
            MapNumbers& mapNumbers,
            FromLatLonToSpherePos& fromLatLonToSpherePos,
            MapSymbols& mapSymbols,
            MapCamera& mapCamera
    );

    std::wstring wname;
public:
    float latitude;
    float longitude;
    float fontSize;
    std::unordered_map<short, void*> visibleZoom;
    uint64_t featureId;
    float invertAnimationUnit = 0;
    float startAnimationElapsedTime = 0;
    uint64_t placeLabelId;
};


#endif //TUSA_ANDROID_MARKERMAPTITLE_H
