//
// Created by Artem on 07.08.2024.
//

#ifndef TUSA_ANDROID_TEXTURE_SHADER_H
#define TUSA_ANDROID_TEXTURE_SHADER_H


#include "shader.h"

class TextureShader : public Shader {
public:
    TextureShader(AAssetManager* assetManager, const char* vertexShaderName, const char* fragmentShaderName);

    GLint getPosLocation() const {return a_pos;}
    GLint getTextureLocation() const {return u_tile_0;}
    GLint getTextureCord() const {return a_unit_square_cords;}

private:
    GLint a_pos;
    GLint a_unit_square_cords;
    GLint u_tile_0;
};


#endif //TUSA_ANDROID_TEXTURE_SHADER_H
