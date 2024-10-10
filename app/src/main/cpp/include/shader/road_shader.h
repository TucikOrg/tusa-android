//
// Created by Artem on 08.10.2024.
//

#ifndef TUSA_ANDROID_ROAD_SHADER_H
#define TUSA_ANDROID_ROAD_SHADER_H

#include "shader.h"
#include <GLES2/gl2.h>

class RoadShader : public Shader {
public:
    RoadShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getPosLocation() const {return a_pos;}
    GLint getEvenLocation() const {return a_even;}
    GLint getPerpendicularsLocation() const {return a_perpendiculars;}
    GLint getUVLocation() const {return a_uv;}
    GLint getShiftVectorLocation() const {return a_shiftVector;}
    GLint getColorLocation() const {return u_color;}
    GLint getPointSizeLocation() const {return u_pointSize;}
    GLint getWidthLocation() const {return u_width;}
    GLint getProjectionLocation() const {return u_projection;}
    GLint getBorderFactorLocation() const {return u_border_factor;}

private:
    GLint a_pos;
    GLint a_even;
    GLint a_perpendiculars;
    GLint a_uv;
    GLint a_shiftVector;

    GLint u_color;
    GLint u_pointSize;
    GLint u_width;
    GLint u_projection;
    GLint u_border_factor;
};


#endif //TUSA_ANDROID_ROAD_SHADER_H
