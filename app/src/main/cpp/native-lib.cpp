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
#include <GLES2/gl2ext.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "util/matrices.h"
#include "util/frustrums.h"
#include "shader/shaders_bucket.h"
#include "gl/cube_example.h"
#include "gl/triangle_example.h"
#include "renderer/renderer.h"
#include "MapRenderer2.h"

#define LOG_TAG "GL_ARTEM"

MapRenderer2 renderer = MapRenderer2();

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_render(JNIEnv *env, jclass clazz) {
    renderer.renderFrame();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_onSurfaceChanged(JNIEnv *env, jclass clazz, jint width,
                                                          jint height) {
    renderer.onSurfaceChanged(width, height);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_surfaceCreated(JNIEnv *env, jclass clazz,
                                                        jobject assetManager) {
    AAssetManager* assetManagerFromJava = AAssetManager_fromJava(env, assetManager);
    renderer.onSurfaceCreated(assetManagerFromJava);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_noOpenGlContextInit(JNIEnv *env, jclass clazz,
                                 jobject assetManager, jobject request_tile) {

    AAssetManager* assetManagerFromJava = AAssetManager_fromJava(env, assetManager);
    //auto globalRequestTile = env->NewGlobalRef(request_tile);
    renderer.init(assetManagerFromJava, env, request_tile);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_drag(JNIEnv *env, jclass clazz, jfloat dx, jfloat dy) {
    renderer.drag(dx, dy);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_scale(JNIEnv *env, jclass clazz, jfloat scale_factor) {
    renderer.scale(scale_factor);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_doubleTap(JNIEnv *env, jclass clazz) {
    //renderer.doubleTap();
    renderer.doubleTap();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_onStop(JNIEnv *env, jclass clazz) {
    //renderer.onStop();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_onDown(JNIEnv *env, jclass clazz) {
    //renderer.onDown();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_addMarker(JNIEnv *env, jobject thiz, jstring key,
                                                   jfloat latitude, jfloat longitude,
                                                   jbyteArray avatar_buffer) {
//    auto key_str = env->GetStringUTFChars(key, 0);
//    jbyte* buffer = env->GetByteArrayElements(avatar_buffer, nullptr);
//    auto* unsignedBuffer = reinterpret_cast<unsigned char*>(buffer);
//    renderer.addMarker(key_str, DEG2RAD(latitude), DEG2RAD(longitude), unsignedBuffer, env->GetArrayLength(avatar_buffer));
//
//    env->ReleaseByteArrayElements(avatar_buffer, buffer, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_updateMarkerGeo(JNIEnv *env, jobject thiz, jstring key,
                                                         jfloat latitude, jfloat longitude) {
//    auto key_str = env->GetStringUTFChars(key, 0);
//    renderer.updateMarkerGeo(key_str, DEG2RAD(latitude), DEG2RAD(longitude));
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_removeMarker(JNIEnv *env, jobject thiz, jstring key) {
//    auto key_str = env->GetStringUTFChars(key, 0);
//    renderer.removeMarker(key_str);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_updateMarkerAvatar(JNIEnv *env, jobject thiz, jstring key,
                                                            jbyteArray avatar_buffer) {
//    auto key_str = env->GetStringUTFChars(key, 0);
//    jbyte* buffer = env->GetByteArrayElements(avatar_buffer, nullptr);
//    auto* unsignedBuffer = reinterpret_cast<unsigned char*>(buffer);
//    renderer.updateMarkerAvatar(key_str , unsignedBuffer, env->GetArrayLength(avatar_buffer));
//
//    env->ReleaseByteArrayElements(avatar_buffer, buffer, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_handleMarker(JNIEnv *env, jobject thiz, jstring key,
                                                      jfloat latitude, jfloat longitude,
                                                      jbyteArray avatar_buffer) {
//    auto key_str = env->GetStringUTFChars(key, 0);
//    jbyte* buffer = env->GetByteArrayElements(avatar_buffer, nullptr);
//    auto* unsignedBuffer = reinterpret_cast<unsigned char*>(buffer);
//    renderer.handleMarker(key_str, DEG2RAD(latitude), DEG2RAD(longitude), unsignedBuffer, env->GetArrayLength(avatar_buffer));
//
//    env->ReleaseByteArrayElements(avatar_buffer, buffer, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_artem_tusaandroid_NativeLibrary_test(JNIEnv *env, jobject thiz, jobject request_tile) {
    // TODO: implement test()
}