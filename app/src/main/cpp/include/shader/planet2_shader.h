//
// Created by Artem on 17.09.2024.
//

#ifndef TUSA_ANDROID_PLANET2_SHADER_H
#define TUSA_ANDROID_PLANET2_SHADER_H


#include "shader.h"

class Planet2Shader : public Shader {
public:
    Planet2Shader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getPosLocation() const {return a_pos;}
    GLint getAnimationLocation() const {return u_animation;}
    GLint getTextureUShiftLocation() const {return u_x_dragged;}
    GLint getTileTextureLocation() const {return u_tile;}
    GLint getUVLocation() const {return a_uv;}

private:
    GLint a_pos;
    GLint u_animation;
    GLint u_x_dragged;
    GLint a_uv;
    GLint u_tile;
};


#endif //TUSA_ANDROID_PLANET2_SHADER_H
