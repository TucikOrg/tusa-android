//
// Created by Artem on 16.10.2024.
//

#ifndef TUSA_ANDROID_MARKERS_H
#define TUSA_ANDROID_MARKERS_H

#include <string>
#include <map>
#include <GLES2/gl2.h>
#include "UserMarker.h"
#include "TextureUtils.h"
#include "shader/shaders_bucket.h"
#include <Eigen/Dense>
#include "util/eigen_gl.h"
#include "MapNumbers.h"
#include "MarkerMapTitle.h"
#include "MapSymbols.h"

class Markers {
public:
    void initGL();
    void addMarker(std::string key, float latitude, float longitude, unsigned char *imageData, off_t fileSize);
    void removeMarker(std::string key);
    void updateMarkerGeo(std::string key, float latitude, float longitude);
    void updateMarkerAvatar(std::string key, unsigned char *imageData, off_t fileSize);
    void drawMarkers(ShadersBucket& shadersBucket,
                     Eigen::Matrix4f pv,
                     MapNumbers& mapNumbers,
                     std::vector<MarkerMapTitle*> markerMapTitles,
                     MapSymbols& mapSymbols,
                     MapCamera& mapCamera
    );
    bool hasMarker(std::string key);
private:
    std::map<std::string, UserMarker> userMarkers = {};

    GLuint titlesVBO;
    GLuint titlesIBO;
    size_t iboSize = 0;
    size_t previousFrameTitlesSize = 0;
};


#endif //TUSA_ANDROID_MARKERS_H
