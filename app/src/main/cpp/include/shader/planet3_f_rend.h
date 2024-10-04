//
// Created by Artem on 04.10.2024.
//

#ifndef TUSA_ANDROID_PLANET3_F_REND_H
#define TUSA_ANDROID_PLANET3_F_REND_H


#include "shader.h"
#include <GLES2/gl2.h>

class Planet3FRendShader : public Shader {
public:
    Planet3FRendShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getPosLocation() const {return a_pos;}
    GLint getColorLocation() const {return u_color;}
    GLint getPointSizeLocation() const {return u_pointSize;}

private:
    GLint a_pos;
    GLint u_color;
    GLint u_pointSize;
};


#endif //TUSA_ANDROID_PLANET3_F_REND_H
