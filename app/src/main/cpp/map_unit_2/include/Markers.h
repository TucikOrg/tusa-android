//
// Created by Artem on 16.10.2024.
//

#ifndef TUSA_ANDROID_MARKERS_H
#define TUSA_ANDROID_MARKERS_H

#include <string>
#include <map>
#include <GLES2/gl2.h>
#include "UserMarker2.h"
#include "TextureUtils.h"
#include "shader/shaders_bucket.h"
#include <Eigen/Dense>

class Markers {
public:
    void addMarker(std::string key, float latitude, float longitude, unsigned char *imageData, off_t fileSize);
    void drawMarkers(ShadersBucket& shadersBucket, Eigen::Matrix4f pvm);
private:
    std::map<std::string, UserMarker2> userMarkers = {};
};


#endif //TUSA_ANDROID_MARKERS_H
