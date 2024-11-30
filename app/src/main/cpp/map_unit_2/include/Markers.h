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
#include "MapTile.h"
#include "DrawTitlesThreadInput.h"

struct WStringHash {
    std::size_t operator()(const std::wstring& str) const {
        std::size_t hash = 0;
        for (wchar_t c : str) {
            hash = hash * 31 + c;
        }
        return hash;
    }
};

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
                     std::unordered_map<uint64_t, MapTile*> tiles,
                     MapSymbols& mapSymbols,
                     MapCamera& mapCamera,
                     MapFpsCounter& mapFpsCounter,
                     bool canRefreshMarkers
    );
    bool hasMarker(std::string key);
    void doubleTap();
private:
    std::map<std::string, UserMarker> userMarkers = {};
    std::unordered_map<uint64_t, MarkerMapTitle*> titleMarkersForRenderStorage = {};

    DrawTitlesThreadInput drawTitlesThreadInput;

    GLuint titlesVBO;
    GLuint titlesIBO;
    size_t iboSize = 0;
    size_t refreshTitlesKey = 0;

    void markersHandleThread();
};


#endif //TUSA_ANDROID_MARKERS_H
