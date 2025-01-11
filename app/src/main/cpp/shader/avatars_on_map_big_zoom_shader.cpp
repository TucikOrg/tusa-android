//
// Created by Artem on 11.01.2025.
//

#include "shader/avatars_on_map_big_zoom_shader.h"


AvatarsOnMapBigZoomShader::AvatarsOnMapBigZoomShader(AAssetManager *assetManager,
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
    u_latLon_previous = glGetUniformLocation(program, "u_latLon_previous");
    u_startMarkerSizeAnimation = glGetUniformLocation(program, "u_startMarkerSizeAnimation");
    u_markerSizeAnimationTime = glGetUniformLocation(program, "u_markerSizeAnimationTime");
    u_matrixPV_SCREEN = glGetUniformLocation(program, "u_matrixPV_SCREEN");
    u_screenSize = glGetUniformLocation(program, "u_screenSize");
    u_animationMarkerType = glGetUniformLocation(program, "u_animationMarkerType");
    u_startSelectionAnimationTime = glGetUniformLocation(program, "u_startSelectionAnimationTime");
    u_startAnimationLatLonTime = glGetUniformLocation(program, "u_startAnimationLatLonTime");
    u_animationLatLonTime = glGetUniformLocation(program, "u_animationLatLonTime");
    u_pointOnMapPrevious = glGetUniformLocation(program, "u_pointOnMapPrevious");
    u_pointOnMap = glGetUniformLocation(program, "u_pointOnMap");
}