//
// Created by Artem on 01.10.2024.
//

#ifndef TUSA_ANDROID_PLANET3_SHADER_H
#define TUSA_ANDROID_PLANET3_SHADER_H

#include "shader.h"

class Planet3Shader : public Shader {
public:
    Planet3Shader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getPosLocation() const {return a_pos;}
    GLint getTextureLocation() const {return u_tex;}
    GLint getPlaneMatrixLocation() const {return u_plane_matrix;}
    GLint getPlanetEPSG3857Location() const {return a_planet_epsg3857;}
    GLint getTransitionLocation() const {return u_transition;}
    GLint getCameraEPSG3857Location() const {return u_cam_epsg3857;}
    GLint getPlaneSizeLocation() const {return u_planeSize;}
    GLint getTextureUVLocation() const {return a_tex_uv;}
    GLint getSphereMatrixLocation() const {return u_sphere_matrix;}
    GLint getUVOffsetLocation() const {return u_uv_offset;}
    GLint getUVScaleLocation() const {return u_uv_scale;}

private:
    GLint a_pos;
    GLint a_planet_epsg3857;
    GLint a_tex_uv;
    GLint u_tex;
    GLint u_transition;
    GLint u_cam_epsg3857;
    GLint u_planeSize;
    GLint u_plane_matrix;
    GLint u_sphere_matrix;
    GLint u_uv_offset;
    GLint u_uv_scale;
};


#endif //TUSA_ANDROID_PLANET3_SHADER_H
