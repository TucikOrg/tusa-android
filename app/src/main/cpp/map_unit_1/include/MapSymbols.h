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
            std::wstring text,
            CSSColorParser::Color color,
            ShadersBucket &shadersBucket,
            MapCamera& mapCamera,
            float symbolScale
    );

    void renderText2D(
            std::wstring text,
            float x,
            float y,
            float symbolScale,
            CSSColorParser::Color color,
            Eigen::Matrix4f matrix,
            ShadersBucket &shadersBucket
    );
    void renderText3D(
            std::wstring text,
            float x,
            float y,
            float z,
            float symbolScale,
            CSSColorParser::Color color,
            Eigen::Matrix4f matrix,
            ShadersBucket &shadersBucket
    );
    TextTexture getTextTexture(std::wstring text);

    Symbol getSymbol(wchar_t c);
private:
    void createFontTextures();
    void prepareWCharForRendering(wchar_t wchar);

    FT_Face face;
    FT_Library ft;
    std::unordered_map<wchar_t, Symbol> symbols = {};
    std::unordered_map<std::wstring, TextTexture> textTextures;

    GLuint framebuffer;
};


#endif //TUSA_ANDROID_MAPSYMBOLS_H
