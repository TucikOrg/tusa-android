//
// Created by Artem on 07.08.2024.
//

#ifndef TUSA_ANDROID_USER_MARKER_SHADER_H
#define TUSA_ANDROID_USER_MARKER_SHADER_H

#include "shader.h"

class UserMarkerShader : public Shader {
public:
    UserMarkerShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getPosLocation() const {return a_pos;}
    GLint getTextureLocation() const {return u_texture;}
    GLint getUVLocation() const {return a_uv;}

private:
    GLint a_pos;
    GLint a_uv;
    GLint u_texture;
};


#endif //TUSA_ANDROID_USER_MARKER_SHADER_H
