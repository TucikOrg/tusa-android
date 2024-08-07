//
// Created by Artem on 06.08.2024.
//

#include "shader/stars_shader.h"


StarsShader::StarsShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName)
        : Shader(assetManager, vertexShaderName, fragmentShaderName) {
    if (!valid) {
        fprintf(stderr, "invalid plain shader\n");
        return;
    }
    a_pos = glGetAttribLocation(program, "vertexPosition");
    a_pointSize = glGetAttribLocation(program, "vertexPointSize");
    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
}
