//
// Created by Artem on 22.09.2024.
//

#ifndef TUSA_ANDROID_MAPSYMBOLS_H
#define TUSA_ANDROID_MAPSYMBOLS_H

#include <GLES2/gl2.h>
#include <android/asset_manager.h>
#include <string>
#include <Eigen/Core>
#include <map>
#include "csscolorparser/csscolorparser.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include "Symbol.h"
#include "shader/shaders_bucket.h"

class MapSymbols {
public:
    ~MapSymbols() {
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    void loadFont(AAssetManager *assetManager);
    void createFontTextures();
    void renderText2D(
            std::string text,
            float x,
            float y,
            float symbolScale,
            CSSColorParser::Color color,
            Eigen::Matrix4f matrix,
            ShadersBucket &shadersBucket
    );
    Symbol getSymbol(char c);
private:
    void prepareCharForRendering(unsigned short charcode);

    bool loadOnlySelectedCharCodes = false;
    std::vector<unsigned short> selectedCharCodesForLoading = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    FT_Face face;
    FT_Library ft;
    std::map<char, Symbol> symbols = {};
};


#endif //TUSA_ANDROID_MAPSYMBOLS_H
