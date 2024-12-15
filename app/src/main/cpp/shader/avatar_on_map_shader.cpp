//
// Created by Artem on 13.12.2024.
//

#include "shader/avatar_on_map_shader.h"


AvatarOnMapShader::AvatarOnMapShader(AAssetManager *assetManager,
                               const char *vertexShaderName,
                               const char *fragmentShaderName) :
        Shader(assetManager, vertexShaderName, fragmentShaderName) {

    if (!valid) {
        fprintf(stderr, "invalid symbol shader\n");
        return;
    }
    a_pos = glGetAttribLocation(program, "vertexPosition");
    a_textureCord = glGetAttribLocation(program, "a_textureCord");
    a_latLon = glGetAttribLocation(program, "a_latLon");
    a_shift = glGetAttribLocation(program, "a_shift");
    a_border_direction = glGetAttribLocation(program, "a_border_direction");
    a_startAnimationElapsedTime = glGetAttribLocation(program, "a_startAnimationElapsedTime");
    a_invertAnimationUnit = glGetAttribLocation(program, "a_invertAnimationUnit");
    a_fontSize = glGetAttribLocation(program, "a_fontSize");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
    u_text = glGetUniformLocation(program, "u_tile_0");
    u_axisLatitude = glGetUniformLocation(program, "u_axisLatitude");
    u_axisLongitude = glGetUniformLocation(program, "u_axisLongitude");
    u_pointOnSphere = glGetUniformLocation(program, "u_pointOnSphere");
    u_current_elapsed_time = glGetUniformLocation(program, "u_current_elapsed_time");
    u_radius = glGetUniformLocation(program, "u_radius");
    u_scale = glGetUniformLocation(program, "u_scale");
    u_border = glGetUniformLocation(program, "u_border");
    u_animationTime = glGetUniformLocation(program, "u_animationTime");
}
