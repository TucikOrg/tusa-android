//
// Created by Artem on 13.12.2024.
//

#ifndef TUSA_ANDROID_AVATAR_ON_MAP_SHADER_H
#define TUSA_ANDROID_AVATAR_ON_MAP_SHADER_H

#include <GLES2/gl2.h>
#include "shader/shader.h"

class AvatarOnMapShader: public Shader {
public:
    AvatarOnMapShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getPosLocation() const {return a_pos;}
    GLint getColorLocation() const {return u_color;}
    GLint getTextureLocation() const {return u_text;}
    GLint getTextureCord() const {return a_textureCord;}
    GLint getLatLonLocation() const {return a_latLon;}
    GLint getTargetMarkerSizeLocation() const {return a_targetMarkerSize;}
    GLint getAxisLatitudeLocation() const {return u_axisLatitude;}
    GLint getAxisLongitudeLocation() const {return u_axisLongitude;}
    GLint getPointOnSphereLocation() const {return u_pointOnSphere;}
    GLint getRadiusLocation() const {return u_radius;}
    GLint getScaleLocation() const {return u_scale;}
    GLint getCurrentElapsedTimeLocation() const {return u_current_elapsed_time;}
    GLint getMovementAnimationTimeLocation() const {return u_movementAnimationTime;}
    GLint getBorderDirection() const {return a_border_direction;}
    GLint getStartAnimationElapsedTimeLocation() const { return a_startAnimationElapsedTime; }
    GLint getAnimationTimeLocation() const { return u_animationTime; }
    GLint getDrawColorMixLocation() const { return u_drawColorMix; }
    GLint getBorderWidthLocation() const { return u_borderWidth; }
    GLint getTargetMarkerSizeArrayLocation() const { return u_targetMarkerSize; }
    GLint getInvertAnimationUnitLocation() const { return a_invertAnimationUnit; }
    GLint getMovementMarkerLocation() const { return a_movementMarker; }
    GLint getMovementTargetMarkerLocation() const { return a_movementTargetMarker; }
    GLint getMovementStartAnimationTimeLocation() const { return a_movementStartAnimationElapsedTime; }

private:
    GLint u_color;
    GLint u_text;
    GLint u_axisLatitude;
    GLint u_axisLongitude;
    GLint u_pointOnSphere;
    GLint u_radius;
    GLint u_scale;
    GLint u_current_elapsed_time;
    GLint u_animationTime;
    GLint u_drawColorMix;
    GLint u_borderWidth;
    GLint u_movementAnimationTime;
    GLint u_targetMarkerSize;

    GLint a_textureCord;
    GLint a_movementStartAnimationElapsedTime;
    GLint a_pos;
    GLint a_startAnimationElapsedTime;
    GLint a_latLon;
    GLint a_border_direction;
    GLint a_invertAnimationUnit;
    GLint a_movementMarker;
    GLint a_movementTargetMarker;
    GLint a_targetMarkerSize;
};


#endif //TUSA_ANDROID_AVATAR_ON_MAP_SHADER_H
