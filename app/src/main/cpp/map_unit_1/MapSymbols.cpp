//
// Created by Artem on 22.09.2024.
//

#include "MapSymbols.h"
#include "util/android_log.h"

void MapSymbols::loadFont(AAssetManager *assetManager) {
    if (FT_Init_FreeType(&ft)) {
        return;
    }

    AAsset* fontFile = AAssetManager_open(assetManager, "fonts/Arial Black.ttf", AASSET_MODE_BUFFER);
    off_t fontDataSize = AAsset_getLength(fontFile);
    FT_Byte *fontData = new FT_Byte[fontDataSize];
    AAsset_read(fontFile, fontData, (size_t) fontDataSize);
    AAsset_close(fontFile);

    if (FT_New_Memory_Face(ft, (const FT_Byte*)fontData, (FT_Long)fontDataSize, 0, &face)){
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 40);
}

void MapSymbols::createFontTextures(MapCamera& mapCamera, ShadersBucket& shadersBucket) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    PrepareCharAtlas prepareCharAtlas = PrepareCharAtlas();

    // Load English characters (ASCII)
    for(unsigned long charcode = 0; charcode <= 255; charcode++) {
        prepareWCharForRendering(charcode, prepareCharAtlas);
    }

    // Load Russian characters (basic range)
    for (unsigned long charcode = 0x0410; charcode <= 0x044F; charcode++) {
        prepareWCharForRendering(charcode, prepareCharAtlas);
    }

    glBindTexture(GL_TEXTURE_2D, charsAtlas);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, prepareCharAtlas.maxTextureWidth, prepareCharAtlas.maxTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, charsAtlas, 0);

    glViewport(0, 0, prepareCharAtlas.maxTextureWidth,  prepareCharAtlas.maxTextureHeight);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    Eigen::Matrix4f projectionTexture = mapCamera.createOrthoProjection(0, prepareCharAtlas.maxTextureWidth, 0, prepareCharAtlas.maxTextureHeight, 0.1, 1);
    Eigen::Matrix4f viewTexture = mapCamera.createView(0, 0, 1, 0, 0, 0);
    Eigen::Matrix4f pvTexture = projectionTexture * viewTexture;

    auto textureShader = shadersBucket.textureShader;
    for (auto symbol : prepareCharAtlas.symbolsToDraw) {
        float points[] = {
                symbol->atlasX, (symbol->atlasY + symbol->rows),
                symbol->atlasX, symbol->atlasY,
                symbol->atlasX + symbol->width, symbol->atlasY,
                symbol->atlasX + symbol->width, (symbol->atlasY + symbol->rows)
        };
        GLfloat textureCords[] = {
                0.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
                1.0f, 0.0f
        };
        unsigned int indices[6] = {
                2, 3, 0,
                0, 1, 2
        };

        glUseProgram(textureShader->program);
        glUniformMatrix4fv(textureShader->getMatrixLocation(), 1, GL_FALSE, pvTexture.data());
        glBindTexture(GL_TEXTURE_2D, symbol->textureId);
        glUniform1i(textureShader->getTextureLocation(), 0);
        glVertexAttribPointer(textureShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, textureCords);
        glEnableVertexAttribArray(textureShader->getTextureCord());
        glVertexAttribPointer(textureShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, points);
        glEnableVertexAttribArray(textureShader->getPosLocation());
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
    }

    atlasWidth = prepareCharAtlas.maxTextureWidth;
    atlasHeight = prepareCharAtlas.maxTextureHeight;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mapCamera.glViewportDeviceSize();
}

void MapSymbols::prepareWCharForRendering(wchar_t wchar, PrepareCharAtlas& prepareCharAtlas) {
    if(FT_Load_Char(face, wchar, FT_LOAD_RENDER)) {
        return;
    }

    unsigned int texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_ALPHA,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_ALPHA,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Symbol symbol = Symbol(
            texture,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            face->glyph->bitmap_left,
            face->glyph->bitmap_top,
            face->glyph->advance.x,
            wchar
    );

    symbols.insert(std::pair<wchar_t, Symbol>(wchar, symbol));
    prepareCharAtlas.add(&symbols[wchar]);
}

void MapSymbols::renderText2D(
    std::wstring text,
    float x,
    float y,
    float symbolScale,
    CSSColorParser::Color color,
    Eigen::Matrix4f matrix,
    ShadersBucket &shadersBucket
) {
    std::vector<std::tuple<Symbol, float, float, float>> forRender {};

    float textWidth = 0;
    float sumHeight = 0;
    for (auto textChar : text) {
        auto symbol = getSymbol(textChar);

        float w = symbol.width * symbolScale;
        float h = symbol.rows * symbolScale;

        float xPixelsShift = (symbol.advance >> 6) * symbolScale;
        textWidth += xPixelsShift;
        sumHeight += h;

        forRender.push_back({symbol, w, h, xPixelsShift});
    }

    float averageHeight = sumHeight / text.length();
    x = x - textWidth / 2;
    y -= averageHeight / 2;

    auto symbolShader = shadersBucket.symbolShader;
    GLfloat red   = static_cast<GLfloat>(color.r) / 255;
    GLfloat green = static_cast<GLfloat>(color.g) / 255;
    GLfloat blue  = static_cast<GLfloat>(color.b) / 255;

    for(auto& charRender : forRender) {
        Symbol symbol = std::get<0>(charRender);
        unsigned int textureId = symbol.textureId;
        float w = std::get<1>(charRender);
        float h = std::get<2>(charRender);
        float pixelsShift = std::get<3>(charRender);
        float xPos = x + symbol.bitmapLeft * symbolScale;
        float yPos = y - (symbol.rows - symbol.bitmapTop) * symbolScale;

        float points[] = {
                xPos, (yPos + h),
                xPos, yPos,
                xPos + w, yPos,
                xPos + w, (yPos + h)
        };
        GLfloat textureCords[] = {
                0.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
                1.0f, 0.0f
        };
        unsigned int indices[6] = {
                2, 3, 0,
                0, 1, 2
        };

        Eigen::Matrix4f identity = Eigen::Matrix4f::Identity();
        glUseProgram(symbolShader->program);
        glUniform3f(symbolShader->getColorLocation(), red, green, blue);
        glUniformMatrix4fv(symbolShader->getProjectionMatrix(), 1, GL_FALSE, identity.data());
        glUniformMatrix4fv(symbolShader->getMatrixLocation(), 1, GL_FALSE, matrix.data());
        glBindTexture(GL_TEXTURE_2D, textureId);
        glUniform1i(symbolShader->getTextureLocation(), 0);
        glVertexAttribPointer(symbolShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, textureCords);
        glEnableVertexAttribArray(symbolShader->getTextureCord());
        glVertexAttribPointer(symbolShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, points);
        glEnableVertexAttribArray(symbolShader->getPosLocation());
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);

        x += pixelsShift;
    }
}

Symbol MapSymbols::getSymbol(wchar_t c) {
    return symbols[c];
}

void MapSymbols::initGl(AAssetManager *assetManager, MapCamera& mapCamera, ShadersBucket& shadersBucket) {
    glGenFramebuffers(1, &framebuffer);
    glGenTextures(1, &charsAtlas);
    createFontTextures(mapCamera, shadersBucket);
}

void MapSymbols::renderText2DByAtlas(
        std::wstring text,
        float xWorld,
        float yWorld,
        float symbolScale,
        CSSColorParser::Color color,
        Eigen::Matrix4f matrix,
        ShadersBucket &shadersBucket
) {
    Eigen::Matrix4f identity = Eigen::Matrix4f::Identity();
    auto atlasW = atlasWidth;
    auto atlasH = atlasHeight;
    std::vector<std::tuple<Symbol, float, float, float>> forRender {};
    auto symbolShader = shadersBucket.symbolShader;
    glUseProgram(symbolShader->program);
    glUniformMatrix4fv(symbolShader->getProjectionMatrix(), 1, GL_FALSE, identity.data());
    glUniformMatrix4fv(symbolShader->getMatrixLocation(), 1, GL_FALSE, matrix.data());
    glUniform4f(symbolShader->getColorLocation(), 1.0, 0.0, 0.0, 1.0);
    glUniform1i(symbolShader->getTextureLocation(), 0);
    GLfloat red   = static_cast<GLfloat>(color.r) / 255;
    GLfloat green = static_cast<GLfloat>(color.g) / 255;
    GLfloat blue  = static_cast<GLfloat>(color.b) / 255;
    glUniform3f(symbolShader->getColorLocation(), red, green, blue);

    float textureWidth = 0;
    float textureHeight = 0;
    float maxTop = 0;
    std::string::const_iterator iterator;
    for (auto charSymbol : text) {
        Symbol symbol = getSymbol(charSymbol);
        float w = symbol.width * symbolScale;
        float h = symbol.rows * symbolScale;
        float top = h - symbol.bitmapTop * symbolScale;
        if (top > maxTop) maxTop = top;

        float xPixelsShift = (symbol.advance >> 6) * symbolScale;
        textureWidth += xPixelsShift;
        if (textureHeight < h + top) textureHeight = h + top;
        forRender.push_back({symbol, w, h, xPixelsShift});
    }

    float halfWidth = textureWidth / 2.0;
    float halfHeight = textureHeight / 2.0;
    float x = 0;
    float y = maxTop;
    for(auto& charRender : forRender) {
        Symbol symbol = std::get<0>(charRender);
        float w = std::get<1>(charRender);
        float h = std::get<2>(charRender);
        float pixelsShift = std::get<3>(charRender);
        float xPos = x + symbol.bitmapLeft * symbolScale + xWorld - halfWidth;
        float yPos = (y - (symbol.rows - symbol.bitmapTop ) * symbolScale) + yWorld - halfHeight;
        float points[] = {
                xPos, yPos,
                xPos + w, yPos,
                xPos + w, (yPos + h),
                xPos, (yPos + h),
        };

        auto startU = symbol.startU(atlasW);
        auto endU = symbol.endU(atlasW);
        auto startV = symbol.startV(atlasH);
        auto endV = symbol.endV(atlasH);
        std::vector<float> textureCords = {
                startU, startV,
                endU, startV,
                endU, endV,
                startU, endV
        };

        glVertexAttribPointer(symbolShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, textureCords.data());
        glEnableVertexAttribArray(symbolShader->getTextureCord());
        glVertexAttribPointer(symbolShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, points);
        glEnableVertexAttribArray(symbolShader->getPosLocation());
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        x += pixelsShift;
    }
}

void MapSymbols::renderText3DByAtlas(
        std::wstring text,
        float xWorld, float yWorld, float zWorld, float symbolScale,
        CSSColorParser::Color color, Eigen::Matrix4f matrix,
        ShadersBucket &shadersBucket) {


    Eigen::Matrix4f identity = Eigen::Matrix4f::Identity();
    auto atlasW = atlasWidth;
    auto atlasH = atlasHeight;
    std::vector<std::tuple<Symbol, float, float, float>> forRender {};
    auto symbolShader = shadersBucket.symbolShader;
    glUseProgram(symbolShader->program);
    glUniformMatrix4fv(symbolShader->getProjectionMatrix(), 1, GL_FALSE, identity.data());
    glUniformMatrix4fv(symbolShader->getMatrixLocation(), 1, GL_FALSE, matrix.data());
    glUniform4f(symbolShader->getColorLocation(), 1.0, 0.0, 0.0, 1.0);
    glUniform1i(symbolShader->getTextureLocation(), 0);
    GLfloat red   = static_cast<GLfloat>(color.r) / 255;
    GLfloat green = static_cast<GLfloat>(color.g) / 255;
    GLfloat blue  = static_cast<GLfloat>(color.b) / 255;
    glUniform3f(symbolShader->getColorLocation(), red, green, blue);

    float textureWidth = 0;
    float textureHeight = 0;
    float maxTop = 0;
    std::string::const_iterator iterator;
    for (auto charSymbol : text) {
        Symbol symbol = getSymbol(charSymbol);
        float w = symbol.width * symbolScale;
        float h = symbol.rows * symbolScale;
        float top = h - symbol.bitmapTop * symbolScale;
        if (top > maxTop) maxTop = top;

        float xPixelsShift = (symbol.advance >> 6) * symbolScale;
        textureWidth += xPixelsShift;
        if (textureHeight < h + top) textureHeight = h + top;
        forRender.push_back({symbol, w, h, xPixelsShift});
    }

    float halfWidth = textureWidth / 2.0;
    float halfHeight = textureHeight / 2.0;
    float x = 0;
    float y = maxTop;
    for(auto& charRender : forRender) {
        Symbol symbol = std::get<0>(charRender);
        float w = std::get<1>(charRender);
        float h = std::get<2>(charRender);
        float pixelsShift = std::get<3>(charRender);
        float xPos = x + symbol.bitmapLeft * symbolScale + xWorld - halfWidth;
        float yPos = (y - (symbol.rows - symbol.bitmapTop ) * symbolScale) + yWorld - halfHeight;
        float zPos = zWorld;
        float points[] = {
                xPos, yPos, zPos,
                xPos + w, yPos, zPos,
                xPos + w, (yPos + h), zPos,
                xPos, (yPos + h), zPos
        };

        auto startU = symbol.startU(atlasW);
        auto endU = symbol.endU(atlasW);
        auto startV = symbol.startV(atlasH);
        auto endV = symbol.endV(atlasH);
        std::vector<float> textureCords = {
                startU, startV,
                endU, startV,
                endU, endV,
                startU, endV
        };

        glVertexAttribPointer(symbolShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, textureCords.data());
        glEnableVertexAttribArray(symbolShader->getTextureCord());
        glVertexAttribPointer(symbolShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, points);
        glEnableVertexAttribArray(symbolShader->getPosLocation());
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        x += pixelsShift;
    }
}



