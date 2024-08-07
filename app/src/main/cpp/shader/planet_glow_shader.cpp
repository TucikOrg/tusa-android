#include <GLES2/gl2.h>
#include "shader/planet_glow_shader.h"
#include "shader/shaders_bucket.h"

PlanetGlowShader::PlanetGlowShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName)
        : Shader(assetManager, vertexShaderName, fragmentShaderName) {
    if (!valid) {
        fprintf(stderr, "invalid planet glow shader\n");
        return;
    }
    a_pos = glGetAttribLocation(program, "vertexPosition");
    a_textureCord = glGetAttribLocation(program, "a_textureCord");
    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
    u_radius = glGetUniformLocation(program, "u_radius");
    u_pointSize = glGetUniformLocation(program, "u_pointSize");
}