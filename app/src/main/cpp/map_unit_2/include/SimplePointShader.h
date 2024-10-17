//
// Created by Artem on 09.10.2024.
//

#ifndef TUSA_ANDROID_SIMPLEPOINTSHADER_H
#define TUSA_ANDROID_SIMPLEPOINTSHADER_H

#include "shader/shader.h"
#include <GLES2/gl2.h>

class SimplePointShader : public Shader {
public:
    SimplePointShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getPosLocation() const {return a_pos;}
    GLint getColorLocation() const {return u_color;}
    GLint getPointSizeLocation() const {return u_pointSize;}

private:
    GLint a_pos;
    GLint u_color;
    GLint u_pointSize;
};


#endif //TUSA_ANDROID_SIMPLEPOINTSHADER_H
