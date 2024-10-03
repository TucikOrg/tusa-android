//
// Created by Artem on 17.09.2024.
//

#include "shader/planet2_shader.h"


Planet2Shader::Planet2Shader(AAssetManager *assetManager, const char *vertexShaderName,
                           const char *fragmentShaderName) : Shader(assetManager, vertexShaderName,
                                                                    fragmentShaderName) {
    if (!valid) {
        fprintf(stderr, "invalid symbol shader\n");
        return;
    }

    u_matrix = glGetUniformLocation(program, "u_matrix");
    a_pos = glGetAttribLocation(program, "a_vertexPosition");
    u_transition = glGetUniformLocation(program, "u_transition");
    a_planet_uv = glGetAttribLocation(program, "a_planet_uv");
    a_tiles_uv = glGetAttribLocation(program, "a_tiles_uv");
    u_tile = glGetUniformLocation(program, "u_tile");
    u_projection = glGetUniformLocation(program, "u_projection");
    u_cam_angle = glGetUniformLocation(program, "u_cam_angle");
    u_plane_camera_y = glGetUniformLocation(program, "u_plane_camera_y");
    u_cam_distance = glGetUniformLocation(program, "u_cam_distance");
    u_radius = glGetUniformLocation(program, "u_radius");
    u_use_poles_zooming = glGetUniformLocation(program, "u_use_poles_zooming");
}