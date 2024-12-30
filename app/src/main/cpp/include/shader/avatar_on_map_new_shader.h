//
// Created by Artem on 26.12.2024.
//

#ifndef TUSA_ANDROID_AVATAR_ON_MAP_NEW_SHADER_H
#define TUSA_ANDROID_AVATAR_ON_MAP_NEW_SHADER_H

#include <GLES2/gl2.h>
#include "shader/shader.h"

class AvatarOnMapNewShader: public Shader {
public:
    AvatarOnMapNewShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getPosLocation() const {return a_pos;}
    GLint getColorLocation() const {return u_color;}
    GLint getTextureLocation() const {return u_text;}
    GLint getTextureCord() const {return a_textureCord;}
    GLint getLatLonLocation() const {return u_latLon;}
    GLint getTargetMarkerSizeLocation() const {return a_targetMarkerSize;}
    GLint getAxisLatitudeLocation() const {return u_axisLatitude;}
    GLint getAxisLongitudeLocation() const {return u_axisLongitude;}
    GLint getPointOnSphereLocation() const {return u_pointOnSphere;}
    GLint getRadiusLocation() const {return u_radius;}
    GLint getScaleLocation() const {return u_scale;}
    GLint getCurrentElapsedTimeLocation() const {return u_current_elapsed_time;}
    GLint getMovementAnimationTimeLocation() const {return u_movementAnimationTime;}
    GLint getBorderDirection() const {return a_border_direction;}
    GLint getStartAnimationElapsedTimeLocation() const { return u_startAnimationElapsedTime; }
    GLint getAnimationTimeLocation() const { return u_animationTime; }
    GLint getDrawColorMixLocation() const { return u_drawColorMix; }
    GLint getBorderWidthLocation() const { return u_borderWidth; }
    GLint getTargetMarkerSizeArrayLocation() const { return u_targetMarkerSize; }
    GLint getInvertAnimationUnitLocation() const { return u_invertAnimationUnit; }
    GLint getMovementMarkerLocation() const { return u_movementMarker; }
    GLint getMovementTargetMarkerLocation() const { return u_movementTargetMarker; }
    GLint getMovementStartAnimationTimeLocation() const { return u_movementStartAnimationTime; }
    GLint getPositionInUniformsLocation() const { return a_positionInUniform; }
    GLint getStartMarkerSizeAnimation() const { return u_startMarkerSizeAnimation; }
    GLint getMarkerSizeAnimationTime() const { return u_markerSizeAnimationTime; }
    GLint getPvScreenLocation() const { return u_matrixPV_SCREEN; }
    GLint getScreenSizeLocation() const { return u_screenSize; }
    GLint getMarkerAnimationType() const { return u_animationMarkerType; }
    GLint getStartSelectionAnimationTime() const { return u_startSelectionAnimationTime; }

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
    GLint u_movementStartAnimationTime;
    GLint u_movementTargetMarker;
    GLint u_startMarkerSizeAnimation;
    GLint u_markerSizeAnimationTime;
    GLint u_animationMarkerType;
    GLint u_startSelectionAnimationTime;

    GLint u_matrixPV_SCREEN;
    GLint u_screenSize;

    GLint a_textureCord;
    GLint a_pos;
    GLint u_startAnimationElapsedTime;
    GLint u_latLon;
    GLint a_border_direction;
    GLint u_invertAnimationUnit;
    GLint u_movementMarker;
    GLint a_targetMarkerSize;
    GLint a_positionInUniform;
};


#endif //TUSA_ANDROID_AVATAR_ON_MAP_NEW_SHADER_H
