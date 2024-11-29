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
                          std::unordered_map<uint64_t, MapTile*> tiles,
                          MapSymbols& mapSymbols,
                          MapCamera& mapCamera
) {
    FromLatLonToSpherePos fromLatLonToSpherePos = FromLatLonToSpherePos();
    fromLatLonToSpherePos.init(mapNumbers);

    std::unordered_map<uint64_t, MarkerMapTitle*> markerMapTitles = {};
    for (auto& tile: tiles) {
        auto tileData = tile.second;
        for (auto& markerTile : tileData->resultMarkerTitles) {
            if (markerMapTitles.find(markerTile.first) != markerMapTitles.end()) {
                continue;
            }
            markerMapTitles[markerTile.first] = &markerTile.second;
        }
    }

    for (auto markerPair: markerMapTitles) {
        auto marker = markerPair.second;
        if (marker->wname == L"Соединённые Штаты") {
            int i = 1;
        }
        titleMarkers[marker->featureId] = marker;
    }

    unsigned int symbolsAmount = 0;
    unsigned int symbolsDataSize = 0;
    std::vector<MarkerMapTitle*> render = {};
    for (auto markerPair : titleMarkers) {
        auto marker = markerPair.second;
        if (marker->wname == L"Соединённые Штаты") {
            int i = 1;
            auto fid = marker->featureId;
            auto& longitude = marker->longitude;
            int n = 0;
        }
        auto& visibleZoom = marker->visibleZoom;
        auto& latitude = marker->latitude;
        auto& longitude = marker->longitude;
        auto& currentZoom = mapNumbers.zoom;
        auto camLatitude = mapNumbers.camLatitude;
        auto camLongitudeNormalized = mapNumbers.camLongitudeNormalized;
        double tooFarDelta = M_PI / pow(2, currentZoom);
        tooFarDelta = M_PI / 2;

        bool visibleZoomSkip = visibleZoom.find(mapNumbers.tileZ) == visibleZoom.end();
        bool tooFarSkip = abs(camLatitude - latitude) > tooFarDelta || abs(camLongitudeNormalized - longitude) > tooFarDelta;
        if (visibleZoomSkip || tooFarSkip) {
            continue;
        }
        symbolsAmount += marker->wname.size();
        symbolsDataSize += marker->wname.size() * 32;

        render.push_back(marker);
    }

    float scale = mapNumbers.scale * 0.016;
    // Если количество маркеров другое то пересоздаем буффера
    if (refreshTitlesKey != symbolsAmount) {
        refreshTitlesKey = symbolsAmount;

        auto atlasW = mapSymbols.atlasWidth;
        auto atlasH = mapSymbols.atlasHeight;
        std::vector<float> symbolsData(symbolsDataSize);
        unsigned int symbolsVerticesIndex = 0;

        for (auto marker : render) {
            auto& text = marker->wname;
            auto& fontSize = marker->fontSize;
            auto& latitude = marker->latitude;
            auto& longitude = marker->longitude;


            float symbolScale = 1.0;
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
                auto startU = symbol.startU(atlasW);
                auto endU = symbol.endU(atlasW);
                auto startV = symbol.startV(atlasH);
                auto endV = symbol.endV(atlasH);
                float data[] = {
                        startU, startV, -latitude, -longitude,  xPos, yPos, -1, -1,
                        endU, startV, -latitude, -longitude,  xPos + w, yPos, 1, -1,
                        endU, endV, -latitude, -longitude,  xPos + w, yPos + h, 1, 1,
                        startU, endV, -latitude, -longitude,  xPos, yPos + h, -1, 1,
                };
                for (float cord : data) {
                    symbolsData[symbolsVerticesIndex++] = cord;
                }

                x += pixelsShift;
            }

            Eigen::Vector3f markerPoint = fromLatLonToSpherePos.getPoint(mapNumbers, latitude, longitude).cast<float>();
            float markerX = markerPoint.x();
            float markerY = markerPoint.y();
            float markerZ = markerPoint.z();
            Eigen::Vector3f leftBottomTextPoint = Eigen::Vector3f { markerX - halfWidth, markerY - halfHeight, markerZ };
            Eigen::Vector3f rightTopTextPoint = Eigen::Vector3f { markerX + halfWidth, markerY + halfHeight, markerZ };

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

        glBindBuffer(GL_ARRAY_BUFFER, titlesVBO);
        auto data = symbolsData.data();
        auto size = symbolsData.size() * sizeof(float);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

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
    glUniform1f(titlesMapShader->getScaleLocation(), scale);
    glUniformMatrix4fv(titlesMapShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
    glUniform1i(titlesMapShader->getTextureLocation(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, titlesVBO);
    glVertexAttribPointer(titlesMapShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(titlesMapShader->getTextureCord());
    glVertexAttribPointer(titlesMapShader->getLatLonLocation(), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(titlesMapShader->getLatLonLocation());
    glVertexAttribPointer(titlesMapShader->getShiftLocation(), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(titlesMapShader->getShiftLocation());
    glVertexAttribPointer(titlesMapShader->getBorderDirection(), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(titlesMapShader->getBorderDirection());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, titlesIBO);
    glUniform1f(titlesMapShader->getBorderLocation(), 0.0);
    glUniform3f(titlesMapShader->getColorLocation(), 1, 1, 1);
    glDrawElements(GL_TRIANGLES, iboSize, GL_UNSIGNED_INT, 0);
    glUniform3f(titlesMapShader->getColorLocation(), 0, 0, 0);
    glUniform1f(titlesMapShader->getBorderLocation(), -2.0);
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

    std::thread markersHandleThread([this] { this->markersHandleThread(); });
    markersHandleThread.detach();
}

void Markers::markersHandleThread() {
    while(true) {

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
