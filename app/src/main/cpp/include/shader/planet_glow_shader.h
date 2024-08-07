//
// Created by Artem on 17.12.2023.
//

#ifndef TUSA_PLANET_GLOW_SHADER_H
#define TUSA_PLANET_GLOW_SHADER_H

#include "shader.h"
#include <GLES2/gl2.h>

class PlanetGlowShader : public Shader {
public:
    PlanetGlowShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getPosLocation() const {return a_pos;}
    GLint getTextureCoordinatesLocation() const {return a_textureCord;}
    GLint getColorLocation() const {return u_color;}
    GLint getPointSizeLocation() const {return u_pointSize;}
    GLint getRadiusLocation() const {return u_radius;}

private:
    GLint a_pos;
    GLint a_textureCord;
    GLint u_color;
    GLint u_pointSize;
    GLint u_radius;
};


#endif //TUSA_PLANET_GLOW_SHADER_H
