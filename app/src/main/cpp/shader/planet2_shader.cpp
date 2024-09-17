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
    u_animation = glGetUniformLocation(program, "u_animation");
    u_x_dragged = glGetUniformLocation(program, "u_x_dragged");
    a_uv = glGetAttribLocation(program, "a_uv");
    u_tile = glGetUniformLocation(program, "u_tile");
}