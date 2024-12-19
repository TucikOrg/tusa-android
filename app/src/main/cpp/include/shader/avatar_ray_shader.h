//
// Created by Artem on 19.12.2024.
//

#ifndef TUSA_ANDROID_AVATAR_RAY_SHADER_H
#define TUSA_ANDROID_AVATAR_RAY_SHADER_H

#include "shader.h"
#include <GLES2/gl2.h>

class AvatarRayShader : public Shader {
public:
    AvatarRayShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getPosLocation() const {return a_pos;}
    GLint getColorLocation() const {return u_color;}
    GLint getPointSizeLocation() const {return u_pointSize;}

private:
    GLint a_pos;
    GLint u_color;
    GLint u_pointSize;
};


#endif //TUSA_ANDROID_AVATAR_RAY_SHADER_H
