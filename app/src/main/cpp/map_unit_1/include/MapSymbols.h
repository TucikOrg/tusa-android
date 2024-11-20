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
#include <vector>
#include "TextTexture.h"
#include "csscolorparser/csscolorparser.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include "Symbol.h"
#include "shader/shaders_bucket.h"
#include "MapCamera.h"
#include "PrepareCharAtlas.h"

class MapSymbols {
public:
    ~MapSymbols() {
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    void initGl(AAssetManager *assetManager, MapCamera& mapCamera, ShadersBucket& shadersBucket);
    void loadFont(AAssetManager *assetManager);
    GLuint getAtlasTexture() { return charsAtlas; }

    void renderText2D(
            std::wstring text,
            float x,
            float y,
            float symbolScale,
            CSSColorParser::Color color,
            Eigen::Matrix4f matrix,
            ShadersBucket &shadersBucket
    );

    void renderText3DByAtlas(
            std::wstring text,
            float x,
            float y,
            float z,
            float symbolScale,
            CSSColorParser::Color color,
            Eigen::Matrix4f matrix,
            ShadersBucket &shadersBucket
    );

    Symbol getSymbol(wchar_t c);

    float atlasWidth;
    float atlasHeight;

private:
    void createFontTextures(MapCamera& mapCamera, ShadersBucket& shadersBucket);
    void prepareWCharForRendering(wchar_t wchar, PrepareCharAtlas& prepareCharAtlas);

    FT_Face face;
    FT_Library ft;
    std::unordered_map<wchar_t, Symbol> symbols = {};

    GLuint framebuffer;
    GLuint charsAtlas;
};


#endif //TUSA_ANDROID_MAPSYMBOLS_H
