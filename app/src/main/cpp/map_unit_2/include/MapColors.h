//
// Created by Artem on 14.10.2024.
//

#ifndef TUSA_ANDROID_MAPCOLORS_H
#define TUSA_ANDROID_MAPCOLORS_H

#include "csscolorparser/csscolorparser.h"
#include <vector>

class MapColors {
public:
    static CSSColorParser::Color getAvatarsBorderColor() {
        return CSSColorParser::parse("rgb(224, 235, 235)");
    }
    static CSSColorParser::Color getStarsColor() {
        return CSSColorParser::parse("rgb(255, 255, 255)");
    }
    static CSSColorParser::Color getSpaceColor() {
        return CSSColorParser::parse("rgb(12, 50, 80)");
    }
    static CSSColorParser::Color getWaterColor() {
        return CSSColorParser::parse("rgba(0, 186, 255, 1.0)");
    }
    static CSSColorParser::Color getLandCoverColor() {
        return CSSColorParser::parse("rgba(103, 234, 44, 0.4f)");
    }
    static CSSColorParser::Color getBackground() {
        return CSSColorParser::parse("rgba(241, 255, 230, 1.0)");
    }
    static std::vector<float> getPoleColor() {
        auto backgroundColor = getBackground();
        float Cd_r = backgroundColor.r / 255.0f;
        float Cd_g = backgroundColor.g / 255.0f;
        float Cd_b = backgroundColor.b / 255.0f;
        float Cd_a = backgroundColor.a;

        auto landCoverColor = getLandCoverColor();
        float Cs_r = landCoverColor.r / 255.0f;
        float Cs_g = landCoverColor.g / 255.0f;
        float Cs_b = landCoverColor.b / 255.0f;
        float Cs_a = landCoverColor.a;
        float Fs = Cs_a;
        float Fd = 1.0 - Cs_a;

        std::vector<float> result = {
               Cs_r * Fs + Cd_r * Fd,
               Cs_g * Fs + Cd_g * Fd,
               Cs_b * Fs + Cd_b * Fd,
               1.0
        };

        return result;
    }
};


#endif //TUSA_ANDROID_MAPCOLORS_H
