//
// Created by Artem on 17.09.2024.
//

#ifndef TUSA_ANDROID_MAPSTYLE_H
#define TUSA_ANDROID_MAPSTYLE_H

#include "csscolorparser/csscolorparser.h"
#include "style/style.h"

class MapStyle {
public:
    unsigned short determineStyle(std::string layerName, layer_map_type props, int z);

    float getLineWidth(unsigned short style) {
        return lineWidth[style];
    }

    float getIsWideLine(unsigned short style) {
        return isWideLine[style];
    }

    CSSColorParser::Color getColor(unsigned short style) {
        return color[style];
    }

    CSSColorParser::Color getBorderColor(unsigned short style) {
        return borderColor[style];
    }

    std::set<unsigned short> getStyles() {
        return styles;
    };

    std::unordered_map<short, void*> getVisibleZoom(unsigned short style) {
        return visibleZoom[style];
    };

    bool getForwardRenderingOnly(unsigned short style) {
        auto iteration = forwardRenderingOnly.find(style);
        if (iteration == forwardRenderingOnly.end()) {
            return false;
        }
        return iteration->second;
    }

    float getBorderFactor(unsigned short style) {
        auto iteration = borderFactor.find(style);
        if (iteration == borderFactor.end()) {
            return false;
        }
        return iteration->second;
    }

    float getAlphaInterpolateFrom(unsigned short style) {
        auto iteration = alphaInterpolateFrom.find(style);
        if (iteration == alphaInterpolateFrom.end()) {
            return -1.0;
        }
        return iteration->second;
    }

    float getAlphaInterpolateTo(unsigned short style) {
        auto iteration = alphaInterpolateTo.find(style);
        if (iteration == alphaInterpolateTo.end()) {
            return -1.0;
        }
        return iteration->second;
    }

    float getRenderWideAfterZoom(unsigned short style) {
        auto iteration = renderWideAfterZoom.find(style);
        if (iteration == renderWideAfterZoom.end()) {
            return 15.0;
        }
        return iteration->second;
    }
private:
    std::set<unsigned short> styles;
    std::map<unsigned short, CSSColorParser::Color> color;
    std::map<unsigned short, CSSColorParser::Color> borderColor;
    std::map<unsigned short, float> lineWidth;
    std::map<unsigned short, float> renderWideAfterZoom;
    std::map<unsigned short, bool> isWideLine;
    std::map<unsigned short, bool> forwardRenderingOnly;
    std::map<unsigned short, float> borderFactor;
    std::map<unsigned short, std::unordered_map<short, void*>> visibleZoom;

    std::map<unsigned short, float> alphaInterpolateFrom;
    std::map<unsigned short, float> alphaInterpolateTo;

    std::unordered_map<short, void*> allZoomsVisible(std::unordered_map<short, void*> expect = {});
    std::unordered_map<short, void*> fromToZoomsVisible(int from, int to);
    bool registerAdminLayer(std::string layerName, uint64_t adminLevel, int& currentIndex);
    bool registerRoadLayer(std::string layerName, std::string className, int& currentIndex);
    bool registerLandUseLayer(std::string layerName, std::string className, int& currentIndex);
};


#endif //TUSA_ANDROID_MAPSTYLE_H
