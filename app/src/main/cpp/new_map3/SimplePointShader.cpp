//
// Created by Artem on 09.10.2024.
//

#include "SimplePointShader.h"
#include "shader/shaders_bucket.h"
#include <GLES2/gl2.h>

SimplePointShader::SimplePointShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName)
        : Shader(assetManager, vertexShaderName, fragmentShaderName) {
    if (!valid) {
        fprintf(stderr, "invalid plain shader\n");
        return;
    }
    a_pos = glGetAttribLocation(program, "vertexPosition");
    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
    u_pointSize = glGetUniformLocation(program, "u_pointSize");
}