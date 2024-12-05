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

bool doIt = true;

void Markers::doubleTap() {
    doIt = !doIt;
}

void Markers::drawMarkers(ShadersBucket& shadersBucket,
                          Eigen::Matrix4f pv,
                          MapNumbers& mapNumbers,
                          std::unordered_map<uint64_t, MapTile*> tiles,
                          MapSymbols& mapSymbols,
                          MapCamera& mapCamera,
                          MapFpsCounter& mapFpsCounter,
                          bool canRefreshMarkers
) {
    FromLatLonToSpherePos fromLatLonToSpherePos = FromLatLonToSpherePos();
    fromLatLonToSpherePos.init(mapNumbers);
    float animationTime = 0.5;
    float scale = mapNumbers.scale;

    float screenWidth = mapCamera.getScreenW();
    float screenHeight = mapCamera.getScreenH();
    std::vector<float> testVertices = {};
    int checks = 0;

    if (canRefreshMarkers) {
        auto& currentZoom = mapNumbers.zoom;
        auto camLatitude = mapNumbers.camLatitude;
        auto camLongitudeNormalized = mapNumbers.camLongitudeNormalized;
        double tooFarDeltaLatitude = M_PI / pow(2, currentZoom);
        double tooFarDeltaLongitude = tooFarDeltaLatitude;
        if (currentZoom < 2.5 && abs(camLatitude) > M_PI / 3) {
            tooFarDeltaLongitude = 2 * M_PI;
            tooFarDeltaLatitude = M_PI / 3;
        }

        auto grid = Grid();
        grid.init(screenWidth, screenHeight, 2, 3);
        // отфильтрованные тайтлы которые нужно показать
        std::unordered_map<uint64_t , MarkerMapTitle*> titlesToRender = {};
        std::unordered_map<uint64_t, void*> handledTitles = {};
        for (auto& tile: tiles) {
            auto tileData = tile.second;
            for (auto& markerTile : tileData->resultMarkerTitles) {
                auto& marker = markerTile.second;
                if (handledTitles.count(marker.placeLabelId)) {
                    continue;
                }
                handledTitles[marker.placeLabelId] = nullptr;
                // test
//                if (marker.wname != L"Ангола" && marker.wname != L"Малазия"
//                    && marker.wname != L"Россия" && marker.wname != L"Исландия"
//                    && marker.wname != L"Монголия" && marker.wname != L"Мали"
//                    ) {
//                    continue;
//                }

                auto& visibleZoom = marker.visibleZoom;
                auto& latitude = marker.latitude;
                auto& longitude = marker.longitude;

                bool visibleZoomSkip = visibleZoom.find(mapNumbers.tileZ) == visibleZoom.end();
                bool tooFarSkip = abs(camLatitude - latitude) > tooFarDeltaLatitude || abs(camLongitudeNormalized - longitude) > tooFarDeltaLongitude;
                if (visibleZoomSkip || tooFarSkip) {
                    continue;
                }

                float scaleCurrentText = scale * marker.fontSize;
                float halfWidth = marker.textWidth / 2.0 * scaleCurrentText;
                float halfHeight = marker.textHeight / 2.0 * scaleCurrentText;
                Eigen::Vector3f markerPoint = fromLatLonToSpherePos.getPoint(mapNumbers, marker);
                float markerX = markerPoint.x();
                float markerY = markerPoint.y();
                float markerZ = markerPoint.z();


                Eigen::Vector4f leftBottomTextPoint = Eigen::Vector4f { markerX - halfWidth, markerY - halfHeight, markerZ, 1.0 };
                Eigen::Vector4f rightTopTextPoint = Eigen::Vector4f { markerX + halfWidth, markerY + halfHeight, markerZ, 1.0 };
                Eigen::Vector4f PClipLeftBottom = pv * leftBottomTextPoint;
                Eigen::Vector4f PClipRightTop = pv * rightTopTextPoint;
                Eigen::Vector3f PNdcLeftBottom;
                PNdcLeftBottom.x() = PClipLeftBottom.x() / PClipLeftBottom.w();
                PNdcLeftBottom.y() = PClipLeftBottom.y() / PClipLeftBottom.w();
                PNdcLeftBottom.z() = PClipLeftBottom.z() / PClipLeftBottom.w();
                Eigen::Vector3f PNdcRightTop;
                PNdcRightTop.x() = PClipRightTop.x() / PClipRightTop.w();
                PNdcRightTop.y() = PClipRightTop.y() / PClipRightTop.w();
                PNdcRightTop.z() = PClipRightTop.z() / PClipRightTop.w();
                float leftBottomScreenX = (PNdcLeftBottom.x() + 1.0f) * 0.5f * screenWidth;
                float leftBottomScreenY = (1.0f - PNdcLeftBottom.y()) * 0.5f * screenHeight;
                float rightTopScreenX = (PNdcRightTop.x() + 1.0f) * 0.5f * screenWidth;
                float rightTopScreenY = (1.0f - PNdcRightTop.y()) * 0.5f * screenHeight;

//                testVertices.push_back(leftBottomScreenX);
//                testVertices.push_back(leftBottomScreenY);
//                testVertices.push_back(rightTopScreenX);
//                testVertices.push_back(rightTopScreenY);

                auto box = Box {
                        static_cast<int>(leftBottomScreenX), static_cast<int>(leftBottomScreenY),
                        static_cast<int>(rightTopScreenX), static_cast<int>(rightTopScreenY),
                        marker.placeLabelId
                };

                bool inserted = grid.insert(box, checks);
                if (inserted == false) {
                    continue;
                }

                titlesToRender[marker.placeLabelId] = &marker;
            }
        }
        grid.clean();

        float elapsedTime = mapFpsCounter.getTimeElapsed();
        bool refresh = false;
        // убираем то что уже не видно
        for (auto it = titleMarkersForRenderStorage.begin(); it != titleMarkersForRenderStorage.end(); ) {
            auto& marker = it->second;

            if (elapsedTime - marker->startAnimationElapsedTime > animationTime && marker->invertAnimationUnit >= 0.5) {
                it = titleMarkersForRenderStorage.erase(it);
                continue;
            }

            auto notRender = titlesToRender.count(it->first) == 0;
            if (notRender && marker->invertAnimationUnit <= 0.5) {
                marker->invertAnimationUnit = 1.0;
                marker->startAnimationElapsedTime = elapsedTime;
                refresh = true;
            }
            ++it;
        }

        // добавляем новое
        for (const auto& [key, marker] : titlesToRender) {
            auto result = titleMarkersForRenderStorage.try_emplace(key, marker);
            if (result.second) {
                marker->invertAnimationUnit = 0.0;
                marker->startAnimationElapsedTime = elapsedTime;
                titleMarkersForRenderStorage[key] = marker;
                refresh = true;
            }
        }


        auto& resultTitles = titleMarkersForRenderStorage;
        // пересоздаем буффер
        if (refresh) {
            unsigned int symbolsAmount = 0;
            unsigned int symbolsDataSize = 0;
            unsigned int symbolsInvertAnimationUnitSize = 0;
            for (auto title : resultTitles) {
                auto& marker = title.second;
                symbolsAmount += marker->wname.size();
                symbolsDataSize += marker->wname.size() * 44;
                symbolsInvertAnimationUnitSize += marker->wname.size() * 8;
            }

            auto atlasW = mapSymbols.atlasWidth;
            auto atlasH = mapSymbols.atlasHeight;
            std::vector<float> symbolsData(symbolsDataSize);
            std::vector<uint8_t> symbolsInvertAnimationUnit(symbolsInvertAnimationUnitSize);
            unsigned int symbolsVerticesIndex = 0;

            for (auto markerPair : resultTitles) {
                auto& marker = markerPair.second;
                auto& startAnimationElapsedTime = marker->startAnimationElapsedTime;
                auto& fontSize = marker->fontSize;
                auto& latitude = marker->latitude;
                auto& longitude = marker->longitude;

                auto& forRender = marker->forRender;
                float textureWidth = marker->textWidth;
                float textureHeight = marker->textHeight;
                float maxTop = marker->maxTop;

                float halfWidth = textureWidth / 2.0;
                float halfHeight = textureHeight / 2.0;
                float x = 0;
                float y = maxTop;
                for(auto& charRender : forRender) {
                    Symbol symbol = std::get<0>(charRender);
                    float w = std::get<1>(charRender);
                    float h = std::get<2>(charRender);
                    float pixelsShift = std::get<3>(charRender);
                    float xPos = x + symbol.bitmapLeft - halfWidth;
                    float yPos = (y - (symbol.rows - symbol.bitmapTop )) - halfHeight;
                    auto startU = symbol.startU(atlasW);
                    auto endU = symbol.endU(atlasW);
                    auto startV = symbol.startV(atlasH);
                    auto endV = symbol.endV(atlasH);
                    float data[] = {
                            startU, startV, -latitude, -longitude,  xPos, yPos, -1, -1, startAnimationElapsedTime, marker->invertAnimationUnit, fontSize,
                            endU, startV, -latitude, -longitude,  xPos + w, yPos, 1, -1, startAnimationElapsedTime, marker->invertAnimationUnit, fontSize,
                            endU, endV, -latitude, -longitude,  xPos + w, yPos + h, 1, 1, startAnimationElapsedTime, marker->invertAnimationUnit, fontSize,
                            startU, endV, -latitude, -longitude,  xPos, yPos + h, -1, 1, startAnimationElapsedTime, marker->invertAnimationUnit, fontSize,
                    };
                    for (float cord : data) {
                        symbolsData[symbolsVerticesIndex++] = cord;
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

            glBindBuffer(GL_ARRAY_BUFFER, titlesVBO);
            auto data = symbolsData.data();
            auto size = symbolsData.size() * sizeof(float);
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, titlesIBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        }
    }

    if (titleMarkersForRenderStorage.size() > 0) {
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
        size_t stride = 11 * sizeof(float);
        glVertexAttribPointer(titlesMapShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, stride, 0);
        glEnableVertexAttribArray(titlesMapShader->getTextureCord());
        glVertexAttribPointer(titlesMapShader->getLatLonLocation(), 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(titlesMapShader->getLatLonLocation());
        glVertexAttribPointer(titlesMapShader->getShiftLocation(), 2, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(float)));
        glEnableVertexAttribArray(titlesMapShader->getShiftLocation());
        glVertexAttribPointer(titlesMapShader->getBorderDirection(), 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(titlesMapShader->getBorderDirection());
        glVertexAttribPointer(titlesMapShader->getStartAnimationElapsedTimeLocation(), 1, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(titlesMapShader->getStartAnimationElapsedTimeLocation());
        glVertexAttribPointer(titlesMapShader->getInvertAnimationUnitLocation(), 1, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
        glEnableVertexAttribArray(titlesMapShader->getInvertAnimationUnitLocation());
        glVertexAttribPointer(titlesMapShader->getFontSizeLocation(), 1, GL_FLOAT, GL_FALSE, stride, (void*)(10 * sizeof(float)));
        glEnableVertexAttribArray(titlesMapShader->getFontSizeLocation());
        glUniform1f(titlesMapShader->getCurrentElapsedTimeLocation(), mapFpsCounter.getTimeElapsed());
        glUniform1f(titlesMapShader->getAnimationTimeLocation(), animationTime);
        glUniform1f(titlesMapShader->getBorderLocation(), 0.0);
        glUniform3f(titlesMapShader->getColorLocation(), 1, 1, 1);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, titlesIBO);
        glDrawElements(GL_TRIANGLES, iboSize, GL_UNSIGNED_INT, 0);
        glUniform3f(titlesMapShader->getColorLocation(), 0, 0, 0);
        glUniform1f(titlesMapShader->getBorderLocation(), -2.0);
        glDrawElements(GL_TRIANGLES, iboSize, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }



//    Eigen::Matrix4f projectionTest = mapCamera.createOrthoProjection(0, screenWidth, screenHeight, 0, 0.1, 2);
//    Eigen::Matrix4f viewTest = mapCamera.createView();
//    Eigen::Matrix4f pvTest = projectionTest * viewTest;
//    auto plainShader = shadersBucket.plainShader;
//    glUseProgram(plainShader->program);
//    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvTest.data());
//    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, testVertices.data());
//    glEnableVertexAttribArray(plainShader->getPosLocation());
//    glUniform4f(plainShader->getColorLocation(), 1.0, 0.0, 0.0, 1.0);
//    glUniform1f(plainShader->getPointSizeLocation(), 20.0f);
//    glDrawArrays(GL_POINTS, 0, testVertices.size() / 2);

//    auto plainShader = shadersBucket.plainShader;
//    glUseProgram(plainShader->program);
//    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
//    glVertexAttribPointer(plainShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, testVertices.data());
//    glEnableVertexAttribArray(plainShader->getPosLocation());
//    glUniform4f(plainShader->getColorLocation(), 1.0, 0.0, 0.0, 1.0);
//    glUniform1f(plainShader->getPointSizeLocation(), 20.0f);
//    glDrawArrays(GL_POINTS, 0, testVertices.size() / 3);


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
}
