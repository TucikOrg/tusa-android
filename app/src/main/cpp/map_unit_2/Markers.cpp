//
// Created by Artem on 16.10.2024.
//

#include "Markers.h"


void Markers::addMarker(
        std::string key,
        float latitude,
        float longitude,
        unsigned char *imageData,
        off_t fileSize
) {
    GLuint textureId = TextureUtils::loadTextureFromBytes(imageData, fileSize);
    userMarkers[key] = {textureId, latitude, longitude};
}

void Markers::drawMarkers(ShadersBucket& shadersBucket, Eigen::Matrix4f pvm) {
    for (auto marker : userMarkers) {
        marker.second.draw(shadersBucket, pvm);
    }
}
