//
// Created by Artem on 18.11.2024.
//

#ifndef TUSA_ANDROID_PREPARECHARATLAS_H
#define TUSA_ANDROID_PREPARECHARATLAS_H

#include <vector>
#include <unordered_map>
#include "Symbol.h"

class PrepareCharAtlas {
public:
    void add(Symbol* symbol) {
        if (maxRowHeight < symbol->rows) {
            maxRowHeight = symbol->rows;
        }

        if (positionX + symbol->width >= maxTextureWidth) {
            positionX = 0;
            positionY += maxRowHeight;
            maxRowHeight = 0;
        }

        symbol->atlasX = positionX;
        symbol->atlasY = positionY;
        symbolsToDraw.push_back(symbol);

        positionX += symbol->width;
    }

    std::vector<Symbol*> symbolsToDraw = {};
    float maxRowHeight = 0;
    float positionX = 0;
    float positionY = 0;

    float maxTextureWidth = 1024;
    float maxTextureHeight = 1024;
};


#endif //TUSA_ANDROID_PREPARECHARATLAS_H
