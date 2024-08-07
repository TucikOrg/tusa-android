//
// Created by Artem on 06.08.2024.
//

#ifndef TUSA_ANDROID_STARS_SHADER_H
#define TUSA_ANDROID_STARS_SHADER_H


#include "shader.h"
#include <GLES2/gl2.h>

class StarsShader : public Shader {
public:
    StarsShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getPosLocation() const {return a_pos;}
    GLint getColorLocation() const {return u_color;}
    GLint getPointSizeLocation() const {return a_pointSize;}

private:
    GLint a_pos;
    GLint a_pointSize;
    GLint u_color;
};


#endif //TUSA_ANDROID_STARS_SHADER_H
