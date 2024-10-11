//
// Created by Artem on 08.10.2024.
//

#include "shader/road_shader.h"

#include <GLES2/gl2.h>
#include "shader/road_shader.h"
#include "shader/shaders_bucket.h"

RoadShader::RoadShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName)
        : Shader(assetManager, vertexShaderName, fragmentShaderName) {
    if (!valid) {
        fprintf(stderr, "invalid plain shader\n");
        return;
    }
    a_pos = glGetAttribLocation(program, "vertexPosition");
    a_even = glGetAttribLocation(program, "a_even");
    a_perpendiculars = glGetAttribLocation(program, "a_perpendiculars");
    a_uv = glGetAttribLocation(program, "a_uv");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
    u_border_color = glGetUniformLocation(program, "u_border_color");
    u_pointSize = glGetUniformLocation(program, "u_pointSize");
    u_width = glGetUniformLocation(program, "u_width");
    u_projection = glGetUniformLocation(program, "u_projection");
    u_border_factor = glGetUniformLocation(program, "u_border_factor");
}
