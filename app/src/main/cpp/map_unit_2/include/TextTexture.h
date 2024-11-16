//
// Created by Artem on 15.11.2024.
//

#ifndef TUSA_ANDROID_TEXTTEXTURE_H
#define TUSA_ANDROID_TEXTTEXTURE_H

#include <GLES2/gl2.h>

class TextTexture {
public:
    float width;
    float height;
    GLuint textureId;

    bool isEmpty() {
        return width == 0 && height == 0 && textureId == 0;
    }
};


#endif //TUSA_ANDROID_TEXTTEXTURE_H
