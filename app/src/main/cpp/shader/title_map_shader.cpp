//
// Created by Artem on 27.11.2024.
//


#include "shader/title_map_shader.h"

TitleMapShader::TitleMapShader(AAssetManager *assetManager,
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
    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
    u_text = glGetUniformLocation(program, "u_tile_0");
    u_projection = glGetUniformLocation(program, "u_projection");
    u_axisLatitude = glGetUniformLocation(program, "u_axisLatitude");
    u_axisLongitude = glGetUniformLocation(program, "u_axisLongitude");
    u_pointOnSphere = glGetUniformLocation(program, "u_pointOnSphere");
    u_radius = glGetUniformLocation(program, "u_radius");
}
