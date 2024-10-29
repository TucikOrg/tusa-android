//
// Created by Artem on 16.10.2024.
//

#include "Markers.h"

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

void Markers::drawMarkers(ShadersBucket& shadersBucket, Eigen::Matrix4f pv, MapNumbers& mapNumbers) {
    double radius = mapNumbers.radius;
    double angleLongitude = -1.0 * mapNumbers.EPSG3857LonNormInf * M_PI;
    double angleLatitude = mapNumbers.EPSG4326CamLat;

    Eigen::AngleAxisd rotationPlanetLatitude = Eigen::AngleAxisd(angleLatitude, Eigen::Vector3d(1, 0, 0));
    Eigen::AngleAxisd rotationPlanetLongitude = Eigen::AngleAxisd(angleLongitude, rotationPlanetLatitude * Eigen::Vector3d(0, 1, 0));
    Eigen::Vector3d axisLatitude = rotationPlanetLongitude * Eigen::Vector3d(1, 0, 0);
    Eigen::Vector3d axisLongitude = rotationPlanetLatitude * Eigen::Vector3d(0, 1, 0);

    for (auto marker : userMarkers) {
        marker.second.draw(
                shadersBucket,
                pv,
                mapNumbers,
                rotationPlanetLatitude,
                rotationPlanetLongitude,
                axisLatitude,
                axisLongitude
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
