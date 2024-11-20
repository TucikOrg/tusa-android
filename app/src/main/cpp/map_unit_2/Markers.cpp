//
// Created by Artem on 16.10.2024.
//

#include "Markers.h"
#include "FromLatLonToSpherePos.h"
#include "MapSymbols.h"

bool Markers::hasMarker(std::string key) {
    return userMarkers.find(key) != userMarkers.end();
}

void Markers::addMarker(
        std::string key,
        float latitude,
        float longitude,
        unsigned char *imageData,
        off_t fileSize
) {
    auto find = userMarkers.find(key);
    if (find != userMarkers.end()) {
        return;
    }
    GLuint textureId = TextureUtils::loadTextureFromBytes(imageData, fileSize);
    userMarkers[key] = { textureId, latitude, longitude };
}

void Markers::drawMarkers(ShadersBucket& shadersBucket,
                          Eigen::Matrix4f pv,
                          MapNumbers& mapNumbers,
                          std::vector<MarkerMapTitle*> markerMapTitles,
                          MapSymbols& mapSymbols,
                          MapCamera& mapCamera
) {
    FromLatLonToSpherePos fromLatLonToSpherePos = FromLatLonToSpherePos();
    fromLatLonToSpherePos.init(mapNumbers);


    glBindTexture(GL_TEXTURE_2D, mapSymbols.getAtlasTexture());
    for (auto marker : markerMapTitles) {
        marker->draw(
                shadersBucket,
                pv,
                mapNumbers,
                fromLatLonToSpherePos,
                mapSymbols,
                mapCamera
        );
    }

    for (auto marker : userMarkers) {
        marker.second.draw(
                shadersBucket,
                pv,
                mapNumbers,
                fromLatLonToSpherePos
        );
    }
}

void Markers::updateMarkerGeo(std::string key, float latitude, float longitude) {
    auto find = userMarkers.find(key);
    if (find == userMarkers.end()) {
        return;
    }
    auto& marker = find->second;
    marker.setPosition(latitude, longitude);
}

void Markers::removeMarker(std::string key) {
    auto find = userMarkers.find(key);
    if (find == userMarkers.end()) {
        return;
    }
    auto& marker = find->second;
    marker.clearTexture();
    userMarkers.erase(find);
}

void Markers::updateMarkerAvatar(std::string key, unsigned char *imageData, off_t fileSize) {
    auto find = userMarkers.find(key);
    if (find == userMarkers.end()) {
        return;
    }
    auto& marker = find->second;
    marker.clearTexture();

    GLuint textureId = TextureUtils::loadTextureFromBytes(imageData, fileSize);
    marker.setTexture(textureId);
}
