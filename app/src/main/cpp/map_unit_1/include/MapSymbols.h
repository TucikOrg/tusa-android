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
#include "TextTexture.h"
#include "csscolorparser/csscolorparser.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include "Symbol.h"
#include "shader/shaders_bucket.h"
#include "MapCamera.h"

class MapSymbols {
public:
    ~MapSymbols() {
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    void initGl(AAssetManager *assetManager);
    void loadFont(AAssetManager *assetManager);

    TextTexture renderTextTexture(
            std::string text,
            CSSColorParser::Color color,
            ShadersBucket &shadersBucket,
            MapCamera& mapCamera,
            float symbolScale
    );

    void renderText2D(
            std::string text,
            float x,
            float y,
            float symbolScale,
            CSSColorParser::Color color,
            Eigen::Matrix4f matrix,
            ShadersBucket &shadersBucket
    );
    void renderText3D(
            std::string text,
            float x,
            float y,
            float z,
            float symbolScale,
            CSSColorParser::Color color,
            Eigen::Matrix4f matrix,
            ShadersBucket &shadersBucket
    );
    TextTexture getTextTexture(std::string text);

    Symbol getSymbol(char c);
private:
    void createFontTextures();
    void prepareCharForRendering(unsigned short charcode);

    bool loadOnlySelectedCharCodes = false;
    std::vector<unsigned short> selectedCharCodesForLoading = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    FT_Face face;
    FT_Library ft;
    std::unordered_map<char, Symbol> symbols = {};
    std::unordered_map<std::string, TextTexture> textTextures;

    GLuint framebuffer;
};


#endif //TUSA_ANDROID_MAPSYMBOLS_H
