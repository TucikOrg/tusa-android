//
// Created by Artem on 13.12.2024.
//

#include "shader/avatar_on_map_shader.h"


AvatarOnMapShader::AvatarOnMapShader(AAssetManager *assetManager,
                               const char *vertexShaderName,
                               const char *fragmentShaderName) :
        Shader(assetManager, vertexShaderName, fragmentShaderName) {

    if (!valid) {
        fprintf(stderr, "invalid symbol shader\n");
        return;
    }
    a_pos = glGetAttribLocation(program, "vertexPosition");
    a_textureCord = glGetAttribLocation(program, "a_textureCord");
    a_border_direction = glGetAttribLocation(program, "a_border_direction");
    a_positionInUniform = glGetAttribLocation(program, "a_positionInUniform");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_color = glGetUniformLocation(program, "u_color");
    u_text = glGetUniformLocation(program, "u_tile_0");
    u_axisLatitude = glGetUniformLocation(program, "u_axisLatitude");
    u_axisLongitude = glGetUniformLocation(program, "u_axisLongitude");
    u_pointOnSphere = glGetUniformLocation(program, "u_pointOnSphere");
    u_current_elapsed_time = glGetUniformLocation(program, "u_current_elapsed_time");
    u_radius = glGetUniformLocation(program, "u_radius");
    u_scale = glGetUniformLocation(program, "u_scale");
    u_animationTime = glGetUniformLocation(program, "u_animationTime");
    u_drawColorMix = glGetUniformLocation(program, "u_drawColorMix");
    u_borderWidth = glGetUniformLocation(program, "u_borderWidth");
    u_movementAnimationTime = glGetUniformLocation(program, "u_movementAnimationTime");
    u_targetMarkerSize = glGetUniformLocation(program, "u_targetMarkerSize");
    u_movementStartAnimationTime = glGetUniformLocation(program, "u_movementStartAnimationTime");
    u_movementTargetMarker = glGetUniformLocation(program, "u_movementTargetMarker");
    u_movementMarker = glGetUniformLocation(program, "u_movementMarker");
    u_invertAnimationUnit = glGetUniformLocation(program, "u_invertAnimationUnit");
    u_startAnimationElapsedTime = glGetUniformLocation(program, "u_startAnimationElapsedTime");
    u_latLon = glGetUniformLocation(program, "u_latLon");
    u_startMarkerSizeAnimation = glGetUniformLocation(program, "u_startMarkerSizeAnimation");
    u_markerSizeAnimationTime = glGetUniformLocation(program, "u_markerSizeAnimationTime");
}
