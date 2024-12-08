//
// Created by Artem on 06.12.2024.
//

#ifndef TUSA_ANDROID_PATH_TEXT_SHADER_H
#define TUSA_ANDROID_PATH_TEXT_SHADER_H

#include "shader.h"
#include <GLES2/gl2.h>

class PathTextShader: public Shader {
public:
    PathTextShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getPosLocation() const {return a_pos;}
    GLint getColorLocation() const {return u_color;}
    GLint getTextureLocation() const {return u_text;}
    GLint getProjectionMatrix() const {return u_projection;}
    GLint getScaleLocation() const {return u_scale;}
    GLint getStartShiftLocation() const {return u_startShift;}
    GLint getTextureCord() const {return a_textureCord;}
    GLint getSkipLenLocation() const {return a_skipLen;}
    GLint getPixelShiftLocation() const {return a_pixelShift;}
    GLint getPathLocation() const {return u_path;}
    GLint getPointsSizeLocation() const {return u_pointsSize;}
    GLint getElapsedTimeLocation() const {return u_elapsedTime;}
    GLint getSymbolShiftLocation() const {return a_symbolShift;}
    GLint getSymbolScaleLocation() const {return u_symbolScale;}

private:
    GLint a_textureCord;
    GLint a_pos;
    GLint a_skipLen;
    GLint a_pixelShift;
    GLint a_symbolShift;

    GLint u_color;
    GLint u_text;
    GLint u_projection;
    GLint u_path;
    GLint u_pointsSize;
    GLint u_elapsedTime;
    GLint u_scale;
    GLint u_startShift;
    GLint u_symbolScale;
};


#endif //TUSA_ANDROID_PATH_TEXT_SHADER_H
