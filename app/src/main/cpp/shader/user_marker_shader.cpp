//
// Created by Artem on 07.08.2024.
//

#include "shader/user_marker_shader.h"

UserMarkerShader::UserMarkerShader(AAssetManager *assetManager, const char *vertexShaderName,
                             const char *fragmentShaderName) : Shader(assetManager, vertexShaderName,
                                                                      fragmentShaderName) {
    if (!valid) {
        fprintf(stderr, "invalid texture shader\n");
        return;
    }

    u_matrix = glGetUniformLocation(program, "u_matrix");
    a_pos = glGetAttribLocation(program, "a_vertexPosition");
    a_uv = glGetAttribLocation(program, "a_uv");
    u_texture = glGetUniformLocation(program, "u_texture");
}
