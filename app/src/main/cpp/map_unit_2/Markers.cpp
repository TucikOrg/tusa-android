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

    // Если количество маркеров другое то пересоздаем буффера
    if (previousFrameTitlesSize != markerMapTitles.size()) {
        previousFrameTitlesSize = markerMapTitles.size();

        unsigned int symbolsAmount = 0;
        unsigned int symbolsVerticesSize = 0;
        unsigned int symbolsUVSize = 0;
        for (auto marker : markerMapTitles) {
            auto& text = marker->wname;
            auto& fontSize = marker->fontSize;
            auto& visibleZoom = marker->visibleZoom;
            auto& latitude = marker->latitude;
            auto& longitude = marker->longitude;
            auto& currentZoom = mapNumbers.zoom;
            auto camLatitude = mapNumbers.camLatitude;
            auto camLongitudeNormalized = mapNumbers.camLongitudeNormalized;
            double tooFarDelta = M_PI / pow(2, currentZoom);

            bool visibleZoomSkip = visibleZoom.find(mapNumbers.tileZ) == visibleZoom.end();
            bool tooFarSkip = abs(camLatitude - latitude) > tooFarDelta || abs(camLongitudeNormalized - longitude) > tooFarDelta;
            if (visibleZoomSkip || tooFarSkip) {
                continue;
            }
            symbolsAmount += marker->wname.size();
            symbolsVerticesSize += marker->wname.size() * 16;
            symbolsUVSize += marker->wname.size() * 8;
        }
        std::vector<float> symbolsVertices(symbolsVerticesSize);
        std::vector<float> symbolsUV(symbolsUVSize);
        unsigned int symbolsVerticesIndex = 0;
        unsigned int symbolsUVIndex = 0;

        for (auto marker : markerMapTitles) {
            auto& text = marker->wname;
            auto& fontSize = marker->fontSize;
            auto& visibleZoom = marker->visibleZoom;
            auto& latitude = marker->latitude;
            auto& longitude = marker->longitude;
            auto& currentZoom = mapNumbers.zoom;
            auto camLatitude = mapNumbers.camLatitude;
            auto camLongitudeNormalized = mapNumbers.camLongitudeNormalized;
            double tooFarDelta = M_PI / pow(2, currentZoom);

            bool visibleZoomSkip = visibleZoom.find(mapNumbers.tileZ) == visibleZoom.end();
            bool tooFarSkip = abs(camLatitude - latitude) > tooFarDelta || abs(camLongitudeNormalized - longitude) > tooFarDelta;
            if (visibleZoomSkip || tooFarSkip) {
                continue;
            }

            float scale = mapNumbers.scale * mapNumbers.distortionDistanceToMapPortion;
            float symbolScale = fontSize * scale;

            auto atlasW = mapSymbols.atlasWidth;
            auto atlasH = mapSymbols.atlasHeight;
            std::vector<std::tuple<Symbol, float, float, float>> forRender {};

            float textureWidth = 0;
            float textureHeight = 0;
            float maxTop = 0;
            std::string::const_iterator iterator;
            for (auto charSymbol : text) {
                Symbol symbol = mapSymbols.getSymbol(charSymbol);
                float w = symbol.width * symbolScale;
                float h = symbol.rows * symbolScale;
                float top = h - symbol.bitmapTop * symbolScale;
                if (top > maxTop) maxTop = top;

                float xPixelsShift = (symbol.advance >> 6) * symbolScale;
                textureWidth += xPixelsShift;
                if (textureHeight < h + top) textureHeight = h + top;
                forRender.push_back({symbol, w, h, xPixelsShift});
            }

            float halfWidth = textureWidth / 2.0;
            float halfHeight = textureHeight / 2.0;
            float x = 0;
            float y = maxTop;
            for(auto& charRender : forRender) {
                Symbol symbol = std::get<0>(charRender);
                float w = std::get<1>(charRender);
                float h = std::get<2>(charRender);
                float pixelsShift = std::get<3>(charRender);
                float xPos = x + symbol.bitmapLeft * symbolScale - halfWidth;
                float yPos = (y - (symbol.rows - symbol.bitmapTop ) * symbolScale) - halfHeight;
                float points[] = {
                        -latitude, -longitude,  xPos, yPos,
                        -latitude, -longitude,  xPos + w, yPos,
                        -latitude, -longitude,  xPos + w, yPos + h,
                        -latitude, -longitude,  xPos, yPos + h,
                };
                for (float cord : points) {
                    symbolsVertices[symbolsVerticesIndex++] = cord;
                }

                auto startU = symbol.startU(atlasW);
                auto endU = symbol.endU(atlasW);
                auto startV = symbol.startV(atlasH);
                auto endV = symbol.endV(atlasH);
                std::vector<float> textureCords = {
                        startU, startV,
                        endU, startV,
                        endU, endV,
                        startU, endV
                };
                for (float cord : textureCords) {
                    symbolsUV[symbolsUVIndex++] = cord;
                }

                x += pixelsShift;
            }
        }

        std::vector<unsigned int> indices(symbolsAmount * 6);
        iboSize = indices.size();
        unsigned int currentPoint = 0;
        for (int i = 0; i < symbolsAmount; i++) {
            unsigned int skip = i * 6;
            indices[skip + 0] = currentPoint + 0;
            indices[skip + 1] = currentPoint + 1;
            indices[skip + 2] = currentPoint + 2;
            indices[skip + 3] = currentPoint + 0;
            indices[skip + 4] = currentPoint + 2;
            indices[skip + 5] = currentPoint + 3;
            currentPoint += 4;
        }

        {
            glBindBuffer(GL_ARRAY_BUFFER, titlesVBO);
            auto data = symbolsVertices.data();
            auto size = symbolsVertices.size() * sizeof(float);
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        }

        {
            glBindBuffer(GL_ARRAY_BUFFER, titlesUvVBO);
            auto data = symbolsUV.data();
            auto size = symbolsUV.size() * sizeof(float);
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, titlesIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }

    CSSColorParser::Color color = CSSColorParser::parse("rgb(0, 0, 0)");
    GLfloat red   = static_cast<GLfloat>(color.r) / 255;
    GLfloat green = static_cast<GLfloat>(color.g) / 255;
    GLfloat blue  = static_cast<GLfloat>(color.b) / 255;
    auto titlesMapShader = shadersBucket.titlesMapShader;
    glUseProgram(titlesMapShader->program);
    Eigen::Vector3f axisLon = fromLatLonToSpherePos.axisLongitude.cast<float>();
    Eigen::Vector3f axisLat = fromLatLonToSpherePos.axisLatitude.cast<float>();
    Eigen::Vector3f pointOnSphere = fromLatLonToSpherePos.pointOnSphere.cast<float>();
    glUniform3f(titlesMapShader->getAxisLongitudeLocation(), axisLon.x(), axisLon.y(), axisLon.z());
    glUniform3f(titlesMapShader->getAxisLatitudeLocation(), axisLat.x(), axisLat.y(), axisLat.z());
    glUniform3f(titlesMapShader->getPointOnSphereLocation(), pointOnSphere.x(), pointOnSphere.y(), pointOnSphere.z());
    glUniform1f(titlesMapShader->getRadiusLocation(), mapNumbers.radius);

    glUniformMatrix4fv(titlesMapShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
    glUniform4f(titlesMapShader->getColorLocation(), 1.0, 0.0, 0.0, 1.0);
    glUniform1i(titlesMapShader->getTextureLocation(), 0);
    glUniform3f(titlesMapShader->getColorLocation(), red, green, blue);
    glBindBuffer(GL_ARRAY_BUFFER, titlesUvVBO);
    glVertexAttribPointer(titlesMapShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(titlesMapShader->getTextureCord());
    glBindBuffer(GL_ARRAY_BUFFER, titlesVBO);
    glVertexAttribPointer(titlesMapShader->getLatLonLocation(), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(titlesMapShader->getLatLonLocation());
    glVertexAttribPointer(titlesMapShader->getShiftLocation(), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(titlesMapShader->getShiftLocation());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, titlesIBO);
    glDrawElements(GL_TRIANGLES, iboSize, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

void Markers::initGL() {
    glGenBuffers(1, &titlesVBO);
    glGenBuffers(1, &titlesIBO);
    glGenBuffers(1, &titlesUvVBO);
}
