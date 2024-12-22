//
// Created by Artem on 19.12.2024.
//

#include "shader/avatar_ray_shader.h"

AvatarRayShader::AvatarRayShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName)
        : Shader(assetManager, vertexShaderName, fragmentShaderName) {
    if (!valid) {
        fprintf(stderr, "invalid plain shader\n");
        return;
    }
    a_latLon = glGetAttribLocation(program, "a_latLon");
    a_startAnimationElapsedTime = glGetAttribLocation(program, "a_startAnimationElapsedTime");
    a_invertAnimationUnit = glGetAttribLocation(program, "a_invertAnimationUnit");
    a_movementMarker = glGetAttribLocation(program, "a_movementMarker");
    a_markerSize = glGetAttribLocation(program, "a_markerSize");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
    u_axisLatitude = glGetUniformLocation(program, "u_axisLatitude");
    u_axisLongitude = glGetUniformLocation(program, "u_axisLongitude");
    u_pointOnSphere = glGetUniformLocation(program, "u_pointOnSphere");
    u_current_elapsed_time = glGetUniformLocation(program, "u_current_elapsed_time");
    u_radius = glGetUniformLocation(program, "u_radius");
    u_scale = glGetUniformLocation(program, "u_scale");
    u_animationTime = glGetUniformLocation(program, "u_animationTime");
}
