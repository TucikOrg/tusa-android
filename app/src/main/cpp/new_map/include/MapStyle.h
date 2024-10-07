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

    CSSColorParser::Color getColor(unsigned short style) {
        return color[style];
    }

    std::set<unsigned short> getStyles() {
        return styles;
    };

private:
    std::set<unsigned short> styles;
    std::map<unsigned short, CSSColorParser::Color> color;
    std::map<unsigned short, float> lineWidth;

    bool registerAdminLayer(std::string layerName, uint64_t adminLevel, int& currentIndex);
};


#endif //TUSA_ANDROID_MAPSTYLE_H
