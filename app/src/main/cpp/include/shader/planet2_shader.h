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
    GLint getTransitionLocation() const {return u_transition;}
    GLint getTileTextureLocation() const {return u_tile;}
    GLint getPlanetUVLocation() const {return a_planet_uv;}
    GLint getTilesUVLocation() const {return a_tiles_uv;}
    GLuint getProjectionMatrixLocation() const {return u_projection;}
    GLuint getCamAngleLocation() const {return u_cam_angle;}
    GLuint getCamDistanceLocation() const {return u_cam_distance;}
    GLuint getRadiusLocation() const {return u_radius;}
    GLuint getPlaneCamYLocation() const {return u_plane_camera_y;}

private:
    GLint a_pos;
    GLint u_transition;
    GLint u_cam_angle;
    GLint a_planet_uv;
    GLint a_tiles_uv;
    GLint u_tile;
    GLint u_projection;
    GLint u_cam_distance;
    GLint u_radius;
    GLint u_plane_camera_y;
};


#endif //TUSA_ANDROID_PLANET2_SHADER_H
