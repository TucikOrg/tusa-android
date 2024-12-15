//
// Created by Artem on 13.12.2024.
//

#ifndef TUSA_ANDROID_AVATARATLASPOINTER_H
#define TUSA_ANDROID_AVATARATLASPOINTER_H

#include <GLES2/gl2.h>

class AvatarAtlasPointer {
public:
    GLuint atlasId;
    int x;
    int y;
    bool active = false;
};


#endif //TUSA_ANDROID_AVATARATLASPOINTER_H
