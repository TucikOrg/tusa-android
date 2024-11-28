//
// Created by Artem on 27.11.2024.
//

#ifndef TUSA_ANDROID_TITLE_MAP_SHADER_H
#define TUSA_ANDROID_TITLE_MAP_SHADER_H

#include <GLES2/gl2.h>
#include "shader/shader.h"

class TitleMapShader: public Shader {
public:
    TitleMapShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getPosLocation() const {return a_pos;}
    GLint getColorLocation() const {return u_color;}
    GLint getTextureLocation() const {return u_text;}
    GLint getTextureCord() const {return a_textureCord;}
    GLint getLatLonLocation() const {return a_latLon;}
    GLint getAxisLatitudeLocation() const {return u_axisLatitude;}
    GLint getAxisLongitudeLocation() const {return u_axisLongitude;}
    GLint getPointOnSphereLocation() const {return u_pointOnSphere;}
    GLint getRadiusLocation() const {return u_radius;}

private:
    GLint a_textureCord;
    GLint a_pos;
    GLint u_color;
    GLint u_text;
    GLint u_axisLatitude;
    GLint u_axisLongitude;
    GLint u_pointOnSphere;
    GLint u_radius;
    GLint a_latLon;
};


#endif //TUSA_ANDROID_TITLE_MAP_SHADER_H
