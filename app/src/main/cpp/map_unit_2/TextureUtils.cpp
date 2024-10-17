//
// Created by Artem on 17.10.2024.
//

#include "TextureUtils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint TextureUtils::loadTextureFromBytes(unsigned char* imageData, long fileSize) {
    int width, height, channels;
    unsigned char* image = stbi_load_from_memory(imageData, fileSize, &width, &height, &channels, STBI_rgb);

    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            width,
            height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            image
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //free(imageData);
    delete[] image;
    return textureId;
}
