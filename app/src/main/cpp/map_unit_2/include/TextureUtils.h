//
// Created by Artem on 17.10.2024.
//

#ifndef TUSA_ANDROID_TEXTUREUTILS_H
#define TUSA_ANDROID_TEXTUREUTILS_H

#include <GLES2/gl2.h>


class TextureUtils {
public:
    static GLuint loadTextureFromBytes(unsigned char* imageData, long fileSize);
    static unsigned char* loadPixels(unsigned char* imageData, long fileSize);
    static void freeImage(unsigned char* image);
};


#endif //TUSA_ANDROID_TEXTUREUTILS_H
