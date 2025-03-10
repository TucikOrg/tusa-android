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
#include "Box.h"
#include "Grid.h"
#include "AvatarAtlasPointer.h"
#include "AvatarsGroup.h"
#include "avatars/Grid.h"
#include "avatars/CircleElement.h"
#include "avatars/CircleNode.h"
#include "avatars/AvatarCollisionShift.h"
#include "avatars/AvatarIntersection.h"

class Markers {
public:
    Markers(MapFpsCounter* mapFpsCounter);

    void initGL();
    void addMarker(int64_t key, float latitude, float longitude, unsigned char *imageData, off_t fileSize, bool updateAvatar);
    void removeMarker(int64_t key);
    void updateMarkerGeo(int64_t key, float latitude, float longitude);
    void updateMarkerAvatar(int64_t key, unsigned char *imageData, off_t fileSize);
    void drawMarkers(ShadersBucket& shadersBucket,
                     Eigen::Matrix4d pv,
                     MapNumbers& mapNumbers,
                     std::unordered_map<uint64_t, MapTile*> tiles,
                     MapSymbols& mapSymbols,
                     MapCamera& mapCamera,
                     bool canRefreshMarkers
    );
    bool hasMarker(int64_t key);
    void doubleTap();
    int64_t confirmedClick(float x, float y, int64_t ignore);
    void deselectSelectedMarker();

    AvatarAtlasPointer nextPlaceForAvatar = AvatarAtlasPointer();
    bool parallelThreadMarkersRunning = true;
    std::thread parallelThreadMarkers;

    void joinThreads() {
        parallelThreadMarkersRunning = false;
        if (parallelThreadMarkers.joinable()) {
            parallelThreadMarkers.join();
        }
    }

    void destroy();

private:
    MapFpsCounter* mapFpsCounter = nullptr;
    int atlasAvatarSize = 2048;
    int avatarSize = 256;

    // сохраненное состояние для параллельного потока
    FromLatLonToSphereDoublePos fromLatLonToSpherePosThread;
    bool lockThread;
    bool manualRefreshAllAvatars = false;
    float screenWidthT;
    float screenHeightT;
    float atlasWidthT;
    float atlasHeightT;
    float radiusT = 0.0;
    float cameraLongitudeT;
    Eigen::Matrix4d pvT;
    std::unordered_map<uint64_t, MapTile*> tilesT;
    float zoomT;
    double camLatitudeT;
    double camLongitudeNormalizedT;
    int tileZT;
    bool canRefreshMarkersT;
    std::vector<float> symbolsDataT;
    std::vector<unsigned int> indicesT;
    size_t iboSize;
    bool symbolDataChanged = false;
    float animationTime = 0.5;

    std::vector<float> testAvatarsVertices = {};
    float scaleT;
    std::vector<Avatars::Circle> circles;
    std::unordered_map<int64_t, uint> circlesMap;
    UserMarker* selectedMarker = nullptr;
    float markerWasSelectedTime = 0.0;



    float borderWidth = 10.0f;
    float arrowBasicHeight = 0.3f;
    float movementAnimationTime = 0.5f;
    float markerSizeAnimationTime = 0.5f;
    float markerAlphaAnimationTime = 0.5f;
    float markerLatLonAnimationTime = 3.0f;
    float minimumMarkerSize = 0.8f;
    float longitudeHideMarkerDelta = M_PI / 2.0;

    std::unordered_map<int64_t, UserMarker> storageMarkers = {};

    std::unordered_map<uint64_t, MarkerMapTitle*> titleMarkersForRenderStorage = {};

    std::vector<int64_t> renderMarkers = {};
    std::unordered_map<int64_t, void*> renderMarkersMap = {};

    DrawTitlesThreadInput drawTitlesThreadInput;
    GLuint frameBuffer;

    GLuint titlesVBO;
    GLuint titlesIBO;
    size_t refreshTitlesKey = 0;

    std::unordered_map<GLuint, void*> refreshGroup = {};
    std::unordered_map<GLuint, AvatarsGroup> avatarsGroups = {};
    size_t refreshAvatarsKey = 0;

    GLuint avatarRaysVBO;
    GLuint avatarsRayIBO;
    size_t avatarsRayIBOSize;

    void updateMarkerAvatarInternal(int64_t& key, unsigned char *imageData, off_t& fileSize);

    void saveNewMarkerPositionAndSize(
            std::unordered_map<int64_t, AvatarCollisionShift>& resultAvatarsShifts,
            std::vector<Avatars::Circle>& circles,
            std::unordered_map<int64_t, uint>& circlesMap,
            float newMovementX,
            float newMovementY,
            float newSize,
            int64_t markerId
    );

    void saveMarkerVisibleState(
            std::unordered_map<int64_t, AvatarCollisionShift>& resultAvatarsShifts,
            float state, int64_t markerId
    );
};


#endif //TUSA_ANDROID_MARKERS_H
