//
// Created by Artem on 19.12.2024.
//

#ifndef TUSA_ANDROID_AVATAR_RAY_SHADER_H
#define TUSA_ANDROID_AVATAR_RAY_SHADER_H

#include "shader.h"
#include <GLES2/gl2.h>

class AvatarRayShader : public Shader {
public:
    AvatarRayShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getColorLocation() const {return u_color;}
    GLint getLatLonLocation() const {return a_latLon;}
    GLint getAxisLatitudeLocation() const {return u_axisLatitude;}
    GLint getAxisLongitudeLocation() const {return u_axisLongitude;}
    GLint getPointOnSphereLocation() const {return u_pointOnSphere;}
    GLint getRadiusLocation() const {return u_radius;}
    GLint getScaleLocation() const {return u_scale;}
    GLint getCurrentElapsedTimeLocation() const {return u_current_elapsed_time;}
    GLint getStartAnimationElapsedTimeLocation() const { return a_startAnimationElapsedTime; }
    GLint getAnimationTimeLocation() const { return u_animationTime; }
    GLint getInvertAnimationUnitLocation() const { return a_invertAnimationUnit; }
    GLint getMovementMarkerLocation() const { return a_movementMarker; }
    GLint getMarkerSizeLocation() const { return a_markerSize; }

private:
    GLint u_color;
    GLint u_axisLatitude;
    GLint u_axisLongitude;
    GLint u_pointOnSphere;
    GLint u_radius;
    GLint u_scale;
    GLint u_current_elapsed_time;
    GLint u_animationTime;

    GLint a_startAnimationElapsedTime;
    GLint a_latLon;
    GLint a_invertAnimationUnit;
    GLint a_movementMarker;
    GLint a_markerSize;
};


#endif //TUSA_ANDROID_AVATAR_RAY_SHADER_H
