//
// Created by Artem on 06.12.2024.
//

#include "shader/path_text_shader.h"


PathTextShader::PathTextShader(AAssetManager *assetManager,
                           const char *vertexShaderName,
                           const char *fragmentShaderName) :
        Shader(assetManager, vertexShaderName, fragmentShaderName) {

    if (!valid) {
        fprintf(stderr, "invalid symbol shader\n");
        return;
    }
    a_pos = glGetAttribLocation(program, "vertexPosition");
    a_textureCord = glGetAttribLocation(program, "a_textureCord");
    a_skipLen = glGetAttribLocation(program, "a_skipLen");
    a_pixelShift = glGetAttribLocation(program, "a_pixelShift");
    a_symbolShift = glGetAttribLocation(program, "a_symbolShift");


    u_symbolScale = glGetUniformLocation(program, "u_symbolScale");
    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
    u_text = glGetUniformLocation(program, "u_tile_0");
    u_projection = glGetUniformLocation(program, "u_projection");
    u_path = glGetUniformLocation(program, "u_path");
    u_pointsSize = glGetUniformLocation(program, "u_pointsSize");
    u_elapsedTime = glGetUniformLocation(program, "u_elapsedTime");
    u_scale = glGetUniformLocation(program, "u_scale");
    u_startShift = glGetUniformLocation(program, "u_startShift");
}