//
// Created by Artem on 16.10.2024.
//

#include "Markers.h"
#include "FromLatLonToSpherePos.h"
#include "MapSymbols.h"
#include "MapColors.h"

Markers::Markers(MapFpsCounter* mapFpsCounter_): mapFpsCounter(mapFpsCounter_) {
    parallelThreadMarkers = std::thread([this] {
        while(parallelThreadMarkersRunning) {
            lockThread = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            if (parallelThreadMarkersRunning == false) break;
            lockThread = true;

            testAvatarsVertices.clear();
            std::unordered_map<int64_t, AvatarCollisionShift> resultAvatarsShifts;

            // делаем выделенный маркер самым главным
            if (selectedMarker != nullptr) {
                bool drawSelectedMarker = renderMarkersMap.count(selectedMarker->markerId) > 0;
                if (drawSelectedMarker) {
                    renderMarkers.erase(std::remove(renderMarkers.begin(),  renderMarkers.end(), selectedMarker->markerId));
                    renderMarkers.insert(renderMarkers.begin(), selectedMarker->markerId);
                }
            }

            // вычисляем пересечения коллизий
            circles.clear();
            circlesMap.clear();
            for (auto& markerId : renderMarkers) {
                auto& marker = storageMarkers[markerId];

                float longitudeDelta = abs(marker.longitude - cameraLongitudeT);
                if (longitudeDelta > longitudeHideMarkerDelta) {
                    saveMarkerVisibleState(resultAvatarsShifts, 1.0, markerId);
                    continue;
                }

                Eigen::Vector3d markerPoint = fromLatLonToSpherePosThread.getPoint(radiusT, marker.latitude, marker.longitude);
                double markerX = markerPoint.x();
                double markerY = markerPoint.y();
                double markerZ = markerPoint.z();

                float markerRadius = UserMarker::defaultMarkerScreenSize;
                Eigen::Vector4d centerAvatarPoint = Eigen::Vector4d { markerX, markerY, markerZ, 1.0 };
                Eigen::Vector4d PClipCenter = pvT * centerAvatarPoint;
                Eigen::Vector3d PNdcCenter;
                PNdcCenter.x() = PClipCenter.x() / PClipCenter.w();
                PNdcCenter.y() = PClipCenter.y() / PClipCenter.w();
                PNdcCenter.z() = PClipCenter.z() / PClipCenter.w();
                int centerScreenX = (PNdcCenter.x() + 1.0) * 0.5 * screenWidthT;
                int centerScreenY = (1.0 - PNdcCenter.y()) * 0.5 * screenHeightT;

                circles.push_back(Avatars::Circle(
                        centerScreenX, centerScreenY, markerRadius,  markerId
                ));
                circlesMap[markerId] = circles.size() - 1;
                circles.back().vectorIndex = circles.size() - 1;
                Avatars::Circle& circle = circles.back();

                int mainIntersectionIndex = 0;
                std::unordered_map<int64_t, void*> ignoreCircles = {};
                ignoreCircles[markerId] = nullptr;


                short inRadialMaxPlaces = 8;
                auto intersections = circle.findIntersections(circles, ignoreCircles, mainIntersectionIndex);
                if (intersections.empty()) {
                    saveNewMarkerPositionAndSize(
                            resultAvatarsShifts, circles, circlesMap,
                            0.0, 0.0,
                            UserMarker::defaultMarkerScreenSize,
                            markerId
                    );
                    saveMarkerVisibleState(resultAvatarsShifts, 0.0, markerId);
                    continue;
                }

                auto& intersectionWithMainMarker = intersections[mainIntersectionIndex];
                auto& mainCircle = circles[circlesMap[intersectionWithMainMarker.markerId]];


                auto directionX = circle.x - mainCircle.realX;
                auto directionY = mainCircle.realY - circle.y;
                auto theMostSuitableRad = abs(atan2(directionY, -directionX) - M_PI);

                // рассчет текущего угла
                float startRad = 0.0f;
                bool isRadialGroup = mainCircle.radialRadStart != -1;
                if (isRadialGroup) {
                    startRad = mainCircle.radialRadStart;
                } else {
                    mainCircle.radialRadStart = theMostSuitableRad;
                    startRad = theMostSuitableRad;
                }
                auto startDeg_DONT_USE = RAD2DEG(startRad);

                if (circle.id == 202) {
                    int i = 0;
                }


                bool freeSpaceFounded = false;
                while(true) {
                    float selectedRad = 0.0;
                    float currentDelta = 2 * M_PI;
                    short selectedPlace = -1;
                    bool hasPlaces = false;
                    for (short place = 0; place < inRadialMaxPlaces; place++) {
                        if (mainCircle.busyPlace.count(place) > 0) {
                            continue;
                        }
                        hasPlaces = true;

                        float checkRad = fmod(startRad + place * 2.0 * M_PI / inRadialMaxPlaces, 2.0 * M_PI);
                        float radDeg_DONT_USE = RAD2DEG(checkRad);

                        float deltaRad = abs(checkRad - theMostSuitableRad);
                        if (currentDelta >= deltaRad) {
                            selectedRad = checkRad;
                            currentDelta = deltaRad;
                            selectedPlace = place;
                        }
                    }
                    if (hasPlaces == false) {
                        break;
                    }

                    float newCircleRadius = UserMarker::minimumMarkerSize;
                    float dxToRootScreen = mainCircle.realX - circle.x;
                    float dyToRootScreen = mainCircle.realY - circle.y;
                    float shiftScreenDistance = newCircleRadius + mainCircle.radius;
                    float movementXRadialScreen = dxToRootScreen + shiftScreenDistance * cos(-selectedRad);
                    float movementYRadialScreen = dyToRootScreen + shiftScreenDistance * sin(-selectedRad);
                    saveNewMarkerPositionAndSize(
                            resultAvatarsShifts, circles, circlesMap,
                            movementXRadialScreen, movementYRadialScreen,
                            newCircleRadius,
                            circle.id
                    );

                    auto intersectionsNew = circle.findIntersections(circles, ignoreCircles, mainIntersectionIndex);
                    mainCircle.busyPlace[selectedPlace] = nullptr;
                    if (intersectionsNew.empty()) {
                        freeSpaceFounded = true;
                        break;
                    }
                }

                if (freeSpaceFounded == false) {
                    saveMarkerVisibleState(resultAvatarsShifts, 1.0, circle.id);
                }

//                if (circle.id == 202) {
//                    int i = 0;
//                    testAvatarsVertices.push_back(circle.realX);
//                    testAvatarsVertices.push_back(circle.realY);
//                    testAvatarsVertices.push_back(mainCircle.realX);
//                    testAvatarsVertices.push_back(mainCircle.realY);
//                }

            }

            // сдвигаем маркеры на экране
            for (auto& pair : resultAvatarsShifts) {
                auto& marker = storageMarkers[pair.first];
                auto shift = pair.second;
                marker.newMovement(
                        refreshGroup,
                        mapFpsCounter,
                        shift.screenDx,
                        shift.screenDY,
                        movementAnimationTime
                );
                marker.newMarkerSize(
                        refreshGroup,
                        mapFpsCounter,
                        shift.screenSize,
                        markerSizeAnimationTime
                );
                marker.visibleState(
                        refreshGroup,
                        mapFpsCounter,
                        shift.visibleState,
                        markerAlphaAnimationTime
                );

                if (pair.first == 253) {
                    int i = 0;
                }

                if (circlesMap.count(pair.first) > 0 && false) {
                    auto circle = circles[circlesMap[pair.first]];
                    testAvatarsVertices.push_back(circle.realX);
                    testAvatarsVertices.push_back(circle.realY);
                }

            }

        }
    });
}

void Markers::doubleTap() {

}

void Markers::drawMarkers(ShadersBucket& shadersBucket,
                          Eigen::Matrix4d pvD,
                          MapNumbers& mapNumbers,
                          std::unordered_map<uint64_t, MapTile*> tiles,
                          MapSymbols& mapSymbols,
                          MapCamera& mapCamera,
                          bool canRefreshMarkers
) {
    Eigen::Matrix4f pv = pvD.cast<float>();
    FromLatLonToSphereDoublePos fromLatLonToSphereDoublePos = FromLatLonToSphereDoublePos();
    fromLatLonToSphereDoublePos.init(mapNumbers);

    float animationTime = 0.5;
    float screenWidth = mapCamera.getScreenW();
    float screenHeight = mapCamera.getScreenH();

    // рендрим текст городов стран и тп
    {
        float scale = mapNumbers.scale * mapNumbers.distortionDistanceToMapPortion;
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
                    Eigen::Vector3f markerPoint = fromLatLonToSphereDoublePos.getPoint(mapNumbers.radius, marker.latitude, marker.longitude).cast<float>();
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


                    auto box = Box (
                            static_cast<int>(leftBottomScreenX), static_cast<int>(leftBottomScreenY),
                            static_cast<int>(rightTopScreenX), static_cast<int>(rightTopScreenY),
                            marker.placeLabelId
                    );

                    bool inserted = grid.insert(box, checks);
                    if (inserted == false) {
                        continue;
                    }

                    titlesToRender[marker.placeLabelId] = &marker;
                }
            }
            grid.clean();

            float elapsedTime = mapFpsCounter->getTimeElapsed();
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
                        // Это тайтлы Надписи на карте
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
            auto titlesMapShader = shadersBucket.titlesMapShader;
            glUseProgram(titlesMapShader->program);
            Eigen::Vector3f axisLon = fromLatLonToSphereDoublePos.axisLongitude.cast<float>();
            Eigen::Vector3f axisLat = fromLatLonToSphereDoublePos.axisLatitude.cast<float>();
            Eigen::Vector3f pointOnSphere = fromLatLonToSphereDoublePos.pointOnSphere.cast<float>();
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
            glUniform1f(titlesMapShader->getCurrentElapsedTimeLocation(), mapFpsCounter->getTimeElapsed());
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
    }


    // рендрим аватары юзеров, маркера
    {
        // создаем свободный атлас для аватаров
        if (nextPlaceForAvatar.active == false) {
            GLuint atlasId;
            glGenTextures(1, &atlasId);
            glBindTexture(GL_TEXTURE_2D, atlasId);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, atlasAvatarSize, atlasAvatarSize, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

            nextPlaceForAvatar.active = true;
            nextPlaceForAvatar.atlasId = atlasId;
            nextPlaceForAvatar.x = 0;
            nextPlaceForAvatar.y = 0;
        }

        // ищем аватары вне атласов
        std::vector<UserMarker> addMeToAtlas = {};
        for (auto pair : storageMarkers) {
            auto marker = pair.second;
            if (marker.uploadedToAtlas) continue;
            addMeToAtlas.push_back(marker);
        }

        // пушим в атлас то чего нету в атласе
        auto& nextPlace = nextPlaceForAvatar;
        if (addMeToAtlas.empty() == false) {
            glBindTexture(GL_TEXTURE_2D, nextPlace.atlasId);
        }
        for (auto& marker : addMeToAtlas) {
            auto pixels = marker.getPixels();
            glTexSubImage2D(GL_TEXTURE_2D, 0, nextPlace.x, nextPlace.y, avatarSize, avatarSize, GL_RGB, GL_UNSIGNED_BYTE, pixels);
            auto& markerPtr = storageMarkers[marker.markerId];
            markerPtr.uploadedToAtlas = true;
            markerPtr.atlasPointer = nextPlaceForAvatar;
            TextureUtils::freeImage(markerPtr.pixels);
            if (avatarsGroups.find(nextPlace.atlasId) == avatarsGroups.end()) {
                GLuint avatarsVBO;
                GLuint avatarsIBO;
                glGenBuffers(1, &avatarsVBO);
                glGenBuffers(1, &avatarsIBO);
                avatarsGroups[nextPlace.atlasId] = { nextPlace.atlasId, {}, avatarsVBO, avatarsIBO };
            }
            avatarsGroups[nextPlace.atlasId].userMarkers.push_back(&markerPtr);

            // сдвигаем место в текстуре дальше
            nextPlace.x += avatarSize;
            if (nextPlace.x + avatarSize > atlasAvatarSize) {
                nextPlace.x = 0;
                nextPlace.y += avatarSize;
            }
            if (nextPlace.y == atlasAvatarSize) {
                // если атлас уже забит под край то на следующем кадре создаем еще один атлас
                // а пока что дальше рендрим только часть аватаров
                nextPlace.active = false;
                break;
            }
        }


        float scale = mapNumbers.scale * mapNumbers.distortionDistanceToMapPortion;
        float elapsedTime = mapFpsCounter->getTimeElapsed();
        bool refreshAll = refreshAvatarsKey != renderMarkers.size() || manualRefreshAllAvatars;
        bool refreshSelectedGroups = refreshGroup.size() > 0;
        if (refreshAll || refreshSelectedGroups) {
            refreshAvatarsKey = renderMarkers.size();
            manualRefreshAllAvatars = false;

            size_t avatarsResultToDrawCount = 0;
            std::vector<float> avatarsRayData = {};

            // cобираем маркера в зависимости от атласов в которых они находятся
            for (auto& pair : avatarsGroups) {
                // если нужно пересобрать только определенные группы
                if (refreshSelectedGroups && refreshGroup.count(pair.first) == 0) {
                    continue;
                }
                refreshGroup.erase(pair.first);

                auto& currentGroup = avatarsGroups[pair.first];
                auto markersOfGroup = pair.second.userMarkers;
                std::vector<float> avatarsData = {};
                std::vector<float> avatarsTargetMarkersSizeUniform(64);
                unsigned int avatarsTargetMarkersSizeUniformIndex = 0;

                std::vector<float> movementStartAnimationUniform(64);
                unsigned int movementStartAnimationUniformIndex = 0;

                std::vector<float> movementTargetLocationUniform(128);
                unsigned int movementTargetLocationUniformIndex = 0;

                std::vector<float> movementLocationUniform(128);
                unsigned int movementLocationUniformIndex = 0;

                std::vector<float> invertAnimationUnitUniform(64);
                unsigned int invertAnimationUnitUniformIndex = 0;

                std::vector<float> latitudeLongitudeUniform(128);
                unsigned int latitudeLongitudeUniformIndex = 0;

                std::vector<float> latitudeLongitudePreviousUniform(128);
                unsigned int latitudeLongitudePreviousUniformIndex = 0;

                std::vector<float> startAnimationElapsedTimeUniform(64);
                unsigned int startAnimationElapsedTimeUniformIndex = 0;

                std::vector<float> startMarkerSizeAnimationTimeUniform(64);
                unsigned int startMarkerSizeAnimationTimeUniformIndex = 0;

                std::vector<float> startAnimationLatLonTimeUniform(64);
                unsigned int startAnimationLatLonTimeUniformIndex = 0;

                std::vector<float> animationTypeUniform(64);
                unsigned int animationTypeUniformIndex = 0;

                float positionInUniforms = 0;
                size_t avatarsToDrawCount = 0;
                for (auto& markerInGroup : markersOfGroup) {
                    auto& marker = markerInGroup;

                    // если маркер не должен быть на экране, но встретился в атласе то игнорируем его
                    if (renderMarkersMap.count(marker->markerId) == 0) {
                        continue;
                    }
                    avatarsToDrawCount++;
                    avatarsResultToDrawCount++;

                    auto& atlasPointer = marker->atlasPointer;
                    auto& markerSize = marker->markerSize;

                    auto& startMarkerSizeAnimationTime = marker->startMarkerSizeAnimationTime;
                    startMarkerSizeAnimationTimeUniform[startMarkerSizeAnimationTimeUniformIndex++] = startMarkerSizeAnimationTime;

                    auto& startAnimationLatLonTime = marker->startAnimationLatLonTime;
                    startAnimationLatLonTimeUniform[startAnimationLatLonTimeUniformIndex++] = startAnimationLatLonTime;

                    auto& latitude = marker->latitude;
                    auto& longitude = marker->longitude;
                    latitudeLongitudeUniform[latitudeLongitudeUniformIndex++] = -latitude;
                    latitudeLongitudeUniform[latitudeLongitudeUniformIndex++] = -longitude;

                    auto& latitudePrevious = marker->latitudePrevious;
                    auto& longitudePrevious = marker->longitudePrevious;
                    latitudeLongitudePreviousUniform[latitudeLongitudePreviousUniformIndex++] = -latitudePrevious;
                    latitudeLongitudePreviousUniform[latitudeLongitudePreviousUniformIndex++] = -longitudePrevious;

                    auto& startAnimationElapsedTime = marker->startAnimationElapsedTime;
                    startAnimationElapsedTimeUniform[startAnimationElapsedTimeUniformIndex++] = startAnimationElapsedTime;

                    float invertAnimationUnit = marker->invertAnimationUnit;
                    invertAnimationUnitUniform[invertAnimationUnitUniformIndex++] = invertAnimationUnit;

                    float movementX = marker->movementX;
                    float movementY = marker->movementY;
                    movementLocationUniform[movementLocationUniformIndex++] = movementX;
                    movementLocationUniform[movementLocationUniformIndex++] = movementY;

                    float movementTargetX = marker->movementTargetX;
                    float movementTargetY = marker->movementTargetY;
                    movementTargetLocationUniform[movementTargetLocationUniformIndex++] = movementTargetX;
                    movementTargetLocationUniform[movementTargetLocationUniformIndex++] = movementTargetY;

                    float movementStartAnimation = marker->startMovementAnimation;
                    movementStartAnimationUniform[movementStartAnimationUniformIndex++] = movementStartAnimation;

                    float targetMarkerSize = marker->targetMarkerSize;
                    avatarsTargetMarkersSizeUniform[avatarsTargetMarkersSizeUniformIndex++] = targetMarkerSize;

                    if (selectedMarker != nullptr && marker->markerId == selectedMarker->markerId) {
                        animationTypeUniform[animationTypeUniformIndex++] = 1.0;
                    } else {
                        animationTypeUniform[animationTypeUniformIndex++] = 0.0;
                    }


                    float startU = FLOAT(atlasPointer.x) / atlasAvatarSize;
                    float endU = FLOAT(atlasPointer.x + avatarSize) / atlasAvatarSize;
                    float startV = FLOAT(atlasPointer.y) / atlasAvatarSize;
                    float endV =  FLOAT(atlasPointer.y + avatarSize) / atlasAvatarSize;

                    float data[] = {
                            startU, startV,  -markerSize, -markerSize,  positionInUniforms,
                            endU, startV,    markerSize, -markerSize,  positionInUniforms,
                            endU, endV,      markerSize, markerSize,   positionInUniforms,
                            startU, endV,    -markerSize, markerSize,   positionInUniforms,
                    };
                    positionInUniforms++;

                    for (auto item : data) {
                        avatarsData.push_back(item);
                    }

                    float avatarsRayRaw[] = {
                            -latitude, -longitude, startAnimationElapsedTime, marker->invertAnimationUnit, 0, 0, 0, markerSize,
                            -latitude, -longitude, startAnimationElapsedTime, marker->invertAnimationUnit, movementX, movementY, -1, markerSize,
                            -latitude, -longitude, startAnimationElapsedTime, marker->invertAnimationUnit, movementX, movementY, 1, markerSize,
                    };
                    for (auto item : avatarsRayRaw) {
                        avatarsRayData.push_back(item);
                    }
                }

                std::vector<unsigned int> indices(avatarsToDrawCount * 6);
                currentGroup.avatarsIboSize = indices.size();
                unsigned int currentPoint = 0;
                for (int i = 0; i < avatarsToDrawCount; i++) {
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
                    glBindBuffer(GL_ARRAY_BUFFER, currentGroup.avatarsVBO);
                    auto data = avatarsData.data();
                    auto size = avatarsData.size() * sizeof(float);
                    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
                }

                currentGroup.avatarsTargetMarkerSize = std::move(avatarsTargetMarkersSizeUniform);
                currentGroup.movementStartAnimation = std::move(movementStartAnimationUniform);
                currentGroup.movementTargetLocation = std::move(movementTargetLocationUniform);
                currentGroup.movementLocation = std::move(movementLocationUniform);
                currentGroup.invertAnimationUnit = std::move(invertAnimationUnitUniform);
                currentGroup.startAnimationElapsedTime = std::move(startAnimationElapsedTimeUniform);
                currentGroup.latitudeLongitude = std::move(latitudeLongitudeUniform);
                currentGroup.latitudeLongitudePrevious = std::move(latitudeLongitudePreviousUniform);
                currentGroup.startMarkerSizeAnimation = std::move(startMarkerSizeAnimationTimeUniform);
                currentGroup.animationType = std::move(animationTypeUniform);
                currentGroup.startAnimationLatLonTime = std::move(startAnimationLatLonTimeUniform);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentGroup.avatarsIBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
            }

            // cобираем лучи
            std::vector<unsigned int> indices(avatarsResultToDrawCount * 3);
            avatarsRayIBOSize = indices.size();
            unsigned int currentPoint = 0;
            for (int i = 0; i < avatarsResultToDrawCount; i++) {
                unsigned int skip = i * 3;
                indices[skip + 0] = currentPoint + 0;
                indices[skip + 1] = currentPoint + 1;
                indices[skip + 2] = currentPoint + 2;
                currentPoint += 3;
            }

            glBindBuffer(GL_ARRAY_BUFFER, avatarRaysVBO);
            auto data = avatarsRayData.data();
            auto size = avatarsRayData.size() * sizeof(float);
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, avatarsRayIBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }


        Eigen::Vector3f axisLon = fromLatLonToSphereDoublePos.axisLongitude.cast<float>();
        Eigen::Vector3f axisLat = fromLatLonToSphereDoublePos.axisLatitude.cast<float>();
        Eigen::Vector3f pointOnSphere = fromLatLonToSphereDoublePos.pointOnSphere.cast<float>();
        auto color = MapColors::getAvatarsBorderColor();

//        if (avatarsRayIBOSize > 0) {
//            // рендрим лучи
//            size_t stride = 8 * sizeof(float);
//
//            auto avatarsRays = shadersBucket.avatarRayShader;
//            glUseProgram(avatarsRays->program);
//            glUniform3f(avatarsRays->getAxisLongitudeLocation(), axisLon.x(), axisLon.y(), axisLon.z());
//            glUniform3f(avatarsRays->getAxisLatitudeLocation(), axisLat.x(), axisLat.y(), axisLat.z());
//            glUniform3f(avatarsRays->getPointOnSphereLocation(), pointOnSphere.x(), pointOnSphere.y(), pointOnSphere.z());
//            glUniform1f(avatarsRays->getRadiusLocation(), mapNumbers.radius);
//            glUniform1f(avatarsRays->getScaleLocation(), scale);
//            glUniformMatrix4fv(avatarsRays->getMatrixLocation(), 1, GL_FALSE, pv.data());
//            glUniform1f(avatarsRays->getCurrentElapsedTimeLocation(), mapFpsCounter->getTimeElapsed());
//            glUniform1f(avatarsRays->getAnimationTimeLocation(), animationTime);
//            glUniform3fv(avatarsRays->getColorLocation(), 1, CommonUtils::toOpenGlColor(color).data());
//
//            glBindBuffer(GL_ARRAY_BUFFER, avatarRaysVBO);
//            glVertexAttribPointer(avatarsRays->getLatLonLocation(), 2, GL_FLOAT, GL_FALSE, stride, (void*)(0 * sizeof(float)));
//            glEnableVertexAttribArray(avatarsRays->getLatLonLocation());
//            glVertexAttribPointer(avatarsRays->getStartAnimationElapsedTimeLocation(), 1, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
//            glEnableVertexAttribArray(avatarsRays->getStartAnimationElapsedTimeLocation());
//            glVertexAttribPointer(avatarsRays->getInvertAnimationUnitLocation(), 1, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
//            glEnableVertexAttribArray(avatarsRays->getInvertAnimationUnitLocation());
//            glVertexAttribPointer(avatarsRays->getMovementMarkerLocation(), 3, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(float)));
//            glEnableVertexAttribArray(avatarsRays->getMovementMarkerLocation());
//            glVertexAttribPointer(avatarsRays->getMarkerSizeLocation(), 1, GL_FLOAT, GL_FALSE, stride, (void*)(7 * sizeof(float)));
//            glEnableVertexAttribArray(avatarsRays->getMarkerSizeLocation());
//            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, avatarsRayIBO);
//            glDrawElements(GL_TRIANGLES, avatarsRayIBOSize, GL_UNSIGNED_INT, 0);
//        }

        Eigen::Matrix4f projectionScreen = mapCamera.createOrthoProjection(0, screenWidth, screenHeight, 0, 0.1, 2);
        Eigen::Matrix4f viewScreen = mapCamera.createView();
        Eigen::Matrix4f pvScreen = projectionScreen * viewScreen;


        auto avatarsOnMap = shadersBucket.avatarOnMapNewShader;
        glUseProgram(avatarsOnMap->program);
        glUniform3f(avatarsOnMap->getAxisLongitudeLocation(), axisLon.x(), axisLon.y(), axisLon.z());
        glUniform3f(avatarsOnMap->getAxisLatitudeLocation(), axisLat.x(), axisLat.y(), axisLat.z());
        glUniform3f(avatarsOnMap->getPointOnSphereLocation(), pointOnSphere.x(), pointOnSphere.y(), pointOnSphere.z());
        glUniform1f(avatarsOnMap->getRadiusLocation(), mapNumbers.radius);
        glUniform1f(avatarsOnMap->getScaleLocation(), scale);
        glUniformMatrix4fv(avatarsOnMap->getMatrixLocation(), 1, GL_FALSE, pv.data());
        glUniformMatrix4fv(avatarsOnMap->getPvScreenLocation(), 1, GL_FALSE, pvScreen.data());
        glUniform2f(avatarsOnMap->getScreenSizeLocation(), screenWidth, screenHeight);
        glUniform1i(avatarsOnMap->getTextureLocation(), 0);
        glUniform1f(avatarsOnMap->getCurrentElapsedTimeLocation(), mapFpsCounter->getTimeElapsed());
        glUniform1f(avatarsOnMap->getAnimationTimeLocation(), animationTime);
        glUniform3fv(avatarsOnMap->getColorLocation(), 1, CommonUtils::toOpenGlColor(color).data());
        glUniform1f(avatarsOnMap->getBorderWidthLocation(), borderWidth);
        glUniform1f(avatarsOnMap->getMarkerSizeAnimationTime(), markerSizeAnimationTime);
        glUniform1f(avatarsOnMap->getStartSelectionAnimationTime(), markerWasSelectedTime);
        glUniform1f(avatarsOnMap->getAnimationLatLonTime(), markerLatLonAnimationTime);

        // рисуем маркера по группам
        for (auto& pair : avatarsGroups) {
            auto& group = pair.second;
            // сколько компонентов в data для рендринга аватара
            size_t stride = 5 * sizeof(float);
            glBindTexture(GL_TEXTURE_2D, pair.first);
            glUniform1fv(avatarsOnMap->getTargetMarkerSizeArrayLocation(), 64, group.avatarsTargetMarkerSize.data());
            glUniform1fv(avatarsOnMap->getMovementStartAnimationTimeLocation(), 64, group.movementStartAnimation.data());
            glUniform2fv(avatarsOnMap->getMovementTargetMarkerLocation(), 64, group.movementTargetLocation.data());
            glUniform2fv(avatarsOnMap->getMovementMarkerLocation(), 64, group.movementLocation.data());
            glUniform1fv(avatarsOnMap->getInvertAnimationUnitLocation(), 64, group.invertAnimationUnit.data());
            glUniform1fv(avatarsOnMap->getStartAnimationElapsedTimeLocation(), 64, group.startAnimationElapsedTime.data());
            glUniform2fv(avatarsOnMap->getLatLonLocation(), 64, group.latitudeLongitude.data());
            glUniform1fv(avatarsOnMap->getStartMarkerSizeAnimation(), 64, group.startMarkerSizeAnimation.data());
            glUniform1fv(avatarsOnMap->getMarkerAnimationType(), 64, group.animationType.data());
            glUniform1fv(avatarsOnMap->getStartAnimationLatLonTime(), 64, group.startAnimationLatLonTime.data());
            glUniform2fv(avatarsOnMap->getLatLonPreviousLocation(), 64, group.latitudeLongitudePrevious.data());

            glBindBuffer(GL_ARRAY_BUFFER, group.avatarsVBO);
            glVertexAttribPointer(avatarsOnMap->getTextureCord(), 2, GL_FLOAT, GL_FALSE, stride, 0);
            glEnableVertexAttribArray(avatarsOnMap->getTextureCord());
            glVertexAttribPointer(avatarsOnMap->getBorderDirection(), 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(avatarsOnMap->getBorderDirection());
            glVertexAttribPointer(avatarsOnMap->getPositionInUniformsLocation(), 1, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(float)));
            glEnableVertexAttribArray(avatarsOnMap->getPositionInUniformsLocation());
            glUniform1f(avatarsOnMap->getMovementAnimationTimeLocation(), movementAnimationTime);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, group.avatarsIBO);
            glUniform1f(avatarsOnMap->getDrawColorMixLocation(), 1.0);
            glDrawElements(GL_TRIANGLES, group.avatarsIboSize, GL_UNSIGNED_INT, 0);
            glUniform1f(avatarsOnMap->getDrawColorMixLocation(), 0.0);
            glDrawElements(GL_TRIANGLES, group.avatarsIboSize, GL_UNSIGNED_INT, 0);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    } // конец рендринга аватаров

    if (lockThread == false) {
        fromLatLonToSpherePosThread = fromLatLonToSphereDoublePos;
        screenWidthT = screenWidth;
        screenHeightT = screenHeight;
        radiusT = mapNumbers.radius;
        pvT = pvD;
        scaleT = mapNumbers.scale * mapNumbers.distortionDistanceToMapPortion;
        cameraLongitudeT = mapNumbers.camLongitude;

        Eigen::Matrix4f projectionTest = mapCamera.createOrthoProjection(0, screenWidth, screenHeight, 0, 0.1, 2);
        Eigen::Matrix4f viewTest = mapCamera.createView();
        Eigen::Matrix4f pvTest = projectionTest * viewTest;
        auto plainShader = shadersBucket.plainShader;
        glUseProgram(plainShader->program);
        glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvTest.data());
        glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, testAvatarsVertices.data());
        glEnableVertexAttribArray(plainShader->getPosLocation());
        glUniform4f(plainShader->getColorLocation(), 1.0, 0.0, 0.0, 1.0);
        glUniform1f(plainShader->getPointSizeLocation(), 20.0f);
        glDrawArrays(GL_POINTS, 0, testAvatarsVertices.size() / 2);
    }

}

void Markers::updateMarkerGeo(int64_t key, float latitude, float longitude) {
    auto find = storageMarkers.find(key);
    if (find == storageMarkers.end()) {
        return;
    }
    auto& marker = find->second;

    float epsilon = 1e-7;
    bool changed = CommonUtils::compareFloats(marker.longitude, longitude, epsilon) == false || CommonUtils::compareFloats(marker.latitude, latitude, epsilon) == false;
    if (changed == false) return;

    marker.setPosition(latitude, longitude, mapFpsCounter, markerLatLonAnimationTime);

    // локация изменилась для этого маркера
    // если этот маркер видимый то пересобираем буффер для рендринга
    if (renderMarkersMap.count(key) == 0) return;

    auto markerGroup = storageMarkers[key].atlasPointer.atlasId;
    refreshGroup[markerGroup] = nullptr;
}

void Markers::removeMarker(int64_t key) {
    // больше его не рендрим но он все равно в памяти
    renderMarkersMap.erase(key);
    renderMarkers.erase(std::remove( renderMarkers.begin(),  renderMarkers.end(), key),  renderMarkers.end());
}

void Markers::updateMarkerAvatar(int64_t key, unsigned char *imageData, off_t fileSize) {
    auto find = storageMarkers.find(key);
    if (find == storageMarkers.end()) {
        return;
    }

    updateMarkerAvatarInternal(key, imageData, fileSize);
    //delete imageData;
}
void Markers::updateMarkerAvatarInternal(int64_t& key, unsigned char *imageData, off_t& fileSize) {
    auto& marker = storageMarkers[key];
    auto& atlasPtr = marker.atlasPointer;
    auto pixels = TextureUtils::loadPixels(imageData, fileSize);
    glBindTexture(GL_TEXTURE_2D, atlasPtr.atlasId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, atlasPtr.x, atlasPtr.y, avatarSize, avatarSize, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    TextureUtils::freeImage(pixels);
}

bool Markers::hasMarker(int64_t key) {
    return storageMarkers.find(key) != storageMarkers.end();
}

void Markers::addMarker(
        int64_t key,
        float latitude,
        float longitude,
        unsigned char *imageData,
        off_t fileSize
) {
    auto find = storageMarkers.find(key);
    if (find != storageMarkers.end()) {
        // если маркер уже есть то обновляем его аватар и рендрим
        updateMarkerAvatarInternal(key, imageData, fileSize);
        renderMarkersMap[key] = nullptr;
        renderMarkers.push_back(key);
        return;
    }

    auto pixels = TextureUtils::loadPixels(imageData, fileSize);
    storageMarkers[key] = UserMarker(pixels, latitude, longitude, key, mapFpsCounter->getTimeElapsed());
    renderMarkersMap[key] = nullptr;
    renderMarkers.push_back(key);

    // тут нужно потестировать с удалением imageData
}

void Markers::initGL() {
    glGenBuffers(1, &titlesVBO);
    glGenBuffers(1, &titlesIBO);
    glGenFramebuffers(1, &frameBuffer);

    glGenBuffers(1, &avatarRaysVBO);
    glGenBuffers(1, &avatarsRayIBO);
}

void Markers::saveMarkerVisibleState(
        std::unordered_map<int64_t, AvatarCollisionShift>& resultAvatarsShifts,
        float state, int64_t markerId
) {
    if (resultAvatarsShifts.count(markerId) == 0) {
        auto& marker = storageMarkers[markerId];
        resultAvatarsShifts[markerId] = AvatarCollisionShift {
                marker.movementTargetX, marker.movementTargetY, marker.targetMarkerSize, state
        };
        return;
    }

    resultAvatarsShifts[markerId].visibleState = state;
}

void Markers::saveNewMarkerPositionAndSize(
        std::unordered_map<int64_t, AvatarCollisionShift>& resultAvatarsShifts,
        std::vector<Avatars::Circle>& circles,
        std::unordered_map<int64_t, uint>& circlesMap,
        float newMovementX,
        float newMovementY,
        float newSize,
        int64_t markerId
) {
    resultAvatarsShifts[markerId] = AvatarCollisionShift {
            newMovementX,
            newMovementY,
            newSize
    };
    auto& circle = circles[circlesMap[markerId]];

    circle.realX = circle.x + newMovementX;
    circle.realY = circle.y + newMovementY;
    circle.radius = newSize;
}

int64_t Markers::confirmedClick(float x, float y, int64_t ignore) {
    unsigned short limit = 20;
    while(lockThread && limit > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        limit--;
    }

    auto circle = Avatars::Circle(x, y, 2, 0);
    std::unordered_map<int64_t, void*> ignoreMarkers = {};
    int mainIntersectionIndex = 0;
    auto intersections = circle.findIntersections(circles, ignoreMarkers, mainIntersectionIndex);
    if (intersections.empty()) {
        return 0;
    }

    auto mainIntersection = intersections[mainIntersectionIndex];
    int64_t markerId = mainIntersection.markerId;
    if (markerId == ignore) {
        return 0;
    }

    selectedMarker = &storageMarkers[markerId];
    refreshGroup[selectedMarker->atlasPointer.atlasId] = nullptr;
    markerWasSelectedTime = mapFpsCounter->getTimeElapsed();
    return markerId;
}

void Markers::deselectSelectedMarker() {
    if (selectedMarker == nullptr) {
        return;
    }

    refreshGroup[selectedMarker->atlasPointer.atlasId] = nullptr;
    selectedMarker = nullptr;
}

void Markers::destroy() {
    glDeleteBuffers(1, &titlesVBO);
    glDeleteBuffers(1, &titlesIBO);
    glDeleteBuffers(1, &avatarRaysVBO);
    glDeleteBuffers(1, &avatarsRayIBO);
    glDeleteFramebuffers(1, &frameBuffer);
    for (auto group : avatarsGroups) {
        auto second = group.second;
        glDeleteBuffers(1, &second.avatarsVBO);
        glDeleteBuffers(1, &second.avatarsIBO);
        glDeleteTextures(1, &second.atlasTexture);
    }
    for (auto& markerPair : storageMarkers) {
        auto& marker = markerPair.second;
    }
}
