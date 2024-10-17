//
// Created by Artem on 10.10.2024.
//

#ifndef TUSA_ANDROID_SQUAREPOINTSHADER_H
#define TUSA_ANDROID_SQUAREPOINTSHADER_H


#include "shader/shader.h"
#include <GLES2/gl2.h>

class SquarePointShader : public Shader {
public:
    SquarePointShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getPosLocation() const {return a_pos;}
    GLint getUVLocation() const {return a_uv;}
    GLint getShiftLocation() const {return a_shift;}
    GLint getColorLocation() const {return u_color;}
    GLint getPointSizeLocation() const {return u_point_size;}
    GLint getProjectionLocation() const {return u_projection;}

private:
    GLint a_pos;
    GLint a_uv;
    GLint a_shift;
    GLint u_color;
    GLint u_point_size;
    GLint u_projection;
};


#endif //TUSA_ANDROID_SQUAREPOINTSHADER_H
