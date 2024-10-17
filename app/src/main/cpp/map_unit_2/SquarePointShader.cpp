//
// Created by Artem on 10.10.2024.
//

#include "SquarePointShader.h"

#include "shader/shaders_bucket.h"
#include <GLES2/gl2.h>

SquarePointShader::SquarePointShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName)
        : Shader(assetManager, vertexShaderName, fragmentShaderName) {
    if (!valid) {
        fprintf(stderr, "invalid square shader\n");
        return;
    }
    a_pos = glGetAttribLocation(program, "vertexPosition");
    a_uv = glGetAttribLocation(program, "a_uv");
    a_shift = glGetAttribLocation(program, "a_shift");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
    u_point_size = glGetUniformLocation(program, "u_point_size");
    u_projection = glGetUniformLocation(program, "u_projection");
}