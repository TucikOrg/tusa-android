//
// Created by Artem on 01.10.2024.
//

#include "shader/planet3_shader.h"

Planet3Shader::Planet3Shader(AAssetManager *assetManager, const char *vertexShaderName,
                             const char *fragmentShaderName) : Shader(assetManager,
                                                                      vertexShaderName,
                                                                      fragmentShaderName) {
    if (!valid) {
        fprintf(stderr, "invalid texture shader\n");
        return;
    }

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_plane_matrix = glGetUniformLocation(program, "u_plane_matrix");
    u_tex = glGetUniformLocation(program, "u_tex");
    u_transition = glGetUniformLocation(program, "u_transition");
    u_cam_epsg3857 = glGetUniformLocation(program, "u_cam_epsg3857");
    u_planeSize = glGetUniformLocation(program, "u_planeSize");
    u_sphere_matrix = glGetUniformLocation(program, "u_sphere_matrix");
    u_uv_offset = glGetUniformLocation(program, "u_uv_offset");
    u_uv_scale = glGetUniformLocation(program, "u_uv_scale");
    u_color = glGetUniformLocation(program, "u_color");

    a_pos = glGetAttribLocation(program, "a_vertexPosition");
    a_planet_epsg3857 = glGetAttribLocation(program, "a_planet_epsg3857");
    a_tex_uv = glGetAttribLocation(program, "a_tex_uv");
}
