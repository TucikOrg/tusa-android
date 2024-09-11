//
// Created by Artem on 01.01.2024.
//

#ifndef TUSA_MAP_H
#define TUSA_MAP_H

#include "../shader/shaders_bucket.h"
#include "renderer/renderer.h"
#include "network/get_tile_request.h"
#include "tiles_storage.h"
#include "style/style.h"
#include "mutex"
#include "gl/open_gl_interface.h"

class Map {
public:
    Map(Cache* cache);

//    Map(const Map&) = delete;
//    Map& operator=(const Map&) = delete;

    void render();
    void onSurfaceChanged(int w, int h);
    void onSurfaceCreated(AAssetManager* assetManager);
    void noOpenGlContextInit(AAssetManager* assetManager, float scaleFactor, JNIEnv *env);
    void drag(float dx, float dy);
    void scale(float scaleFactor);
    void doubleTap();
    void onStop();
    void onDown();
    void addMarker(std::string key, float latitude, float longitude, unsigned char* imageData, off_t fileSize);
    void updateMarkerGeo(std::string key, float latitude, float longitude);
    void removeMarker(std::string key);
    void updateMarkerAvatar(std::string key, unsigned char *imageData, off_t fileSize);
    void handleMarker(std::string key, float latitude, float longitude, unsigned char *imageData, off_t fileSize);
private:
    float zoom = 0;
    Cache* cache;
    std::shared_ptr<ShadersBucket> shadersBucket = std::shared_ptr<ShadersBucket>(new ShadersBucket());
    Renderer renderer = Renderer(shadersBucket, cache);
};


#endif //TUSA_MAP_H
