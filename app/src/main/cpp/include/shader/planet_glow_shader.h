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
    GLint getUVLocation() const {return a_uv;}
    GLint getColorLocation() const {return u_color;}
    GLint getPointSizeLocation() const {return u_pointSize;}

private:
    GLint a_pos;
    GLint a_uv;
    GLint u_color;
    GLint u_pointSize;
};


#endif //TUSA_PLANET_GLOW_SHADER_H
