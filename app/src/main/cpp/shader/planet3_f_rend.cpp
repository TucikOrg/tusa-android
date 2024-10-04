//
// Created by Artem on 04.10.2024.
//

#include "shader/planet3_f_rend.h"
#include <GLES2/gl2.h>
#include "shader/shaders_bucket.h"

Planet3FRendShader::Planet3FRendShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName)
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
