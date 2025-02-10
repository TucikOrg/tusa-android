//
// Created by Artem on 11.11.2024.
//

#ifndef TUSA_ANDROID_MARKERMAPTITLE_H
#define TUSA_ANDROID_MARKERMAPTITLE_H

#include <string>
#include "shader/shaders_bucket.h"
#include "MapNumbers.h"
#include "MapSymbols.h"
#include <Eigen/Dense>
#include "IGetPointData.h"


class MarkerMapTitle: public IGetPointData {
public:
    MarkerMapTitle() {}
    MarkerMapTitle(
        std::wstring wname,
        float latitude,
        float longitude,
        float fontSize,
        std::unordered_map<short, void*> visibleZoom,
        uint64_t placeLabelId,
        float textWidth,
        float textHeight,
        std::vector<std::tuple<Symbol, float, float, float>> forRender,
        float maxTop,
        uint64_t filterNumber,
        uint64_t id
    ):
        wname(wname),
        latitude(latitude),
        longitude(longitude),
        fontSize(fontSize),
        visibleZoom(visibleZoom),
        placeLabelId(placeLabelId),
        textWidth(textWidth),
        textHeight(textHeight),
        forRender(forRender),
        maxTop(maxTop),
        filterNumber(filterNumber),
        id(id)
    {
        cos_minus_lat = cos(-latitude);
        sin_minus_lat = sin(-latitude);
        sin_lon = sin(longitude);
        cos_lon = cos(longitude);
    }

    std::wstring wname;
public:
    float latitude;
    float longitude;
    float fontSize;
    std::unordered_map<short, void*> visibleZoom;
    float invertAnimationUnit = 0;
    float startAnimationElapsedTime = 0;
    uint64_t placeLabelId;
    float textWidth;
    float textHeight;
    std::vector<std::tuple<Symbol, float, float, float>> forRender;
    float maxTop;
    uint64_t filterNumber;
    uint64_t id;
};


#endif //TUSA_ANDROID_MARKERMAPTITLE_H
