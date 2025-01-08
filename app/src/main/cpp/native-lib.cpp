#include <jni.h>
#include <string>
#include <iostream>
#include <cstdio>
#include <csetjmp>
#include <vector>
#include <cmath>
#include <sstream>
#include <iosfwd>

#include <android/log.h>
#include <cstdlib>
#include <GLES2/gl2.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "shader/shaders_bucket.h"
#include "MapRenderer.h"

#define LOG_TAG "GL_ARTEM"

MapRenderer* renderer = new MapRenderer();

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_create(JNIEnv *env, jclass clazz) {
    if (renderer == nullptr) {
        renderer = new MapRenderer();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_cleanup(JNIEnv *env, jclass clazz) {
    if (renderer != nullptr) {
        renderer->cleanup();
        delete renderer;
        renderer = nullptr;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_render(JNIEnv *env, jclass clazz) {
    renderer->renderFrame();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_onSurfaceChanged(JNIEnv *env, jclass clazz, jint width,
                                                          jint height) {
    renderer->onSurfaceChanged(width, height);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_surfaceCreated(JNIEnv *env, jclass clazz,
                                                        jobject assetManager) {
    AAssetManager* assetManagerFromJava = AAssetManager_fromJava(env, assetManager);

    renderer->onSurfaceCreated(assetManagerFromJava);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_surfaceDestroyed(JNIEnv *env, jclass clazz,
                                                        jobject assetManager) {
    renderer->destroyGL();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_noOpenGlContextInit(JNIEnv *env, jclass clazz,
                                 jobject assetManager, jobject request_tile) {

    AAssetManager* assetManagerFromJava = AAssetManager_fromJava(env, assetManager);
    //auto globalRequestTile = env->NewGlobalRef(request_tile);
    renderer->init(assetManagerFromJava, env, request_tile);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_drag(JNIEnv *env, jclass clazz, jfloat dx, jfloat dy) {
    renderer->drag(dx, dy);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_scale(JNIEnv *env, jclass clazz, jfloat scale_factor) {
    renderer->scale(scale_factor);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_doubleTap(JNIEnv *env, jclass clazz) {
    renderer->doubleTap();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_onDown(JNIEnv *env, jclass clazz) {
    //renderer.onDown();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_updateMarkerGeo(JNIEnv *env, jobject thiz, jlong key,
                                                         jfloat latitude, jfloat longitude) {
    int64_t keyLong = static_cast<int64_t>(key);
    renderer->getMarkers().updateMarkerGeo(keyLong, DEG2RAD(latitude), DEG2RAD(longitude));
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_removeMarker(JNIEnv *env, jobject thiz, jlong key) {
    int64_t keyLong = static_cast<int64_t>(key);
    renderer->getMarkers().removeMarker(keyLong);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_updateMarkerAvatar(JNIEnv *env, jobject thiz, jlong key,
                                                            jbyteArray avatar_buffer) {
    jbyte* buffer = env->GetByteArrayElements(avatar_buffer, nullptr);
    auto* unsignedBuffer = reinterpret_cast<unsigned char*>(buffer);
    int64_t keyLong = static_cast<int64_t>(key);
    renderer->getMarkers().updateMarkerAvatar(keyLong, unsignedBuffer, env->GetArrayLength(avatar_buffer));

    env->ReleaseByteArrayElements(avatar_buffer, buffer, 0);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_addMarker(JNIEnv *env, jobject thiz, jlong key,
                                                      jfloat latitude, jfloat longitude,
                                                      jbyteArray avatar_buffer) {
    int64_t keyLong = static_cast<int64_t>(key);
    jbyte* buffer = env->GetByteArrayElements(avatar_buffer, nullptr);
    auto* unsignedBuffer = reinterpret_cast<unsigned char*>(buffer);
    auto& markers = renderer->getMarkers();
    markers.addMarker(keyLong, DEG2RAD(latitude), DEG2RAD(longitude), unsignedBuffer, env->GetArrayLength(avatar_buffer));

    env->ReleaseByteArrayElements(avatar_buffer, buffer, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_test(JNIEnv *env, jobject thiz, jobject request_tile) {
    // TODO: implement test()
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_artem_tusaandroid_NativeLibrary_existMarker(JNIEnv *env, jobject thiz, jlong key) {
    int64_t keyLong = static_cast<int64_t>(key);
    auto& markers = renderer->getMarkers();
    auto result = markers.hasMarker(keyLong);
    return result;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_setCameraPos(JNIEnv *env, jobject thiz, jfloat latitude,
                                                      jfloat longitude, jfloat zoom) {
    auto& mapControls = renderer->getMapControls();
    mapControls.setCamPos(DEG2RAD(latitude), DEG2RAD(longitude));
    mapControls.setZoom(zoom);
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_artem_tusaandroid_NativeLibrary_getCameraPos(JNIEnv *env, jobject thiz) {
    auto& mapControls = renderer->getMapControls();
    auto camPos = mapControls.getCamPos();
    jclass cls = env->FindClass("com/artem/tusaandroid/location/CameraPos");
    jmethodID constructor = env->GetMethodID(cls, "<init>", "(FFF)V");
    jobject obj = env->NewObject(cls, constructor, (float) RAD2DEG(camPos.latitude), (float) RAD2DEG(camPos.longitude), camPos.zoom);
    return obj;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_artem_tusaandroid_NativeLibrary_confirmedClick(JNIEnv *env, jobject thiz, jfloat x, jfloat y, jlong ignore) {
    auto selectedMarkerID = renderer->confirmedClick(x, y, ignore);
    jclass cls = env->FindClass("com/artem/tusaandroid/MapClickResult");
    jmethodID constructor = env->GetMethodID(cls, "<init>", "(J)V");
    jobject obj = env->NewObject(cls, constructor, selectedMarkerID);
    return obj;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_deselectSelectedMarker(JNIEnv *env, jobject thiz) {
    renderer->deselectSelectedMarker();
}