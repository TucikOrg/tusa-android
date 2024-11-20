//
// Created by Artem on 17.10.2024.
//

#ifndef TUSA_ANDROID_SYMBOL_H
#define TUSA_ANDROID_SYMBOL_H


#include "ft2build.h"
#include FT_FREETYPE_H

class Symbol {
public:
    Symbol();
    Symbol(
            unsigned int textureId,
            unsigned int width,
            unsigned int rows,
            FT_Int bitmapLeft,
            FT_Int bitmapTop,
            FT_Pos advance,
            wchar_t symbol
    );

    unsigned int textureId;
    unsigned int width;
    unsigned int rows;
    FT_Int bitmapLeft;
    FT_Int bitmapTop;
    FT_Pos advance;

    float atlasX = 0;
    float atlasY = 0;
    wchar_t symbol;

    float startU(float maxW) {
        return atlasX / maxW;
    }

    float endU(float maxW) {
        return (atlasX + width) / maxW;
    }

    float startV(float maxH) {
        return atlasY / maxH;
    }

    float endV(float maxH) {
        return (atlasY + rows) / maxH;
    }
};


#endif //TUSA_ANDROID_SYMBOL_H
