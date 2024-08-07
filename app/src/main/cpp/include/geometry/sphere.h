//
// Created by Artem on 20.04.2024.
//

#ifndef TUSA_ANDROID_SPHERE_H
#define TUSA_ANDROID_SPHERE_H

#include <math.h>
#include <cmath>
#include <vector>
#include "map/mercator.h"
#include "util/android_log.h"

class Sphere {
public:
    void generateSphereData(
            int tileX_start,
            int tileX_end,
            int tileY_start,
            int tileY_end,
            int zoom,
            int tileGrain,
            float radius
            ) {

        // clear memory of prev arrays
        std::vector<float>().swap(sphere_vertices);
        std::vector<unsigned int>().swap(sphere_indices);
        std::vector<float>().swap(normals);
        std::vector<float>().swap(unitSquareCoordinates);

        int n = pow(2, zoom);
        bool isEdge = tileX_start > tileX_end;
        int xFrom = 0;
        int xTo = 0;
        int xFromEdge = 0;
        int xToEdge = 0;
        int xTilesCount = 0;
        int yTilesCount = tileY_end - tileY_start + 1;
        if (isEdge) {
            xFrom = tileX_start;
            xTo = n - 1;
            xFromEdge = 0;
            xToEdge = tileX_end;
            xTilesCount = (n - tileX_start) + tileX_end + 1;
        } else {
            xTilesCount = tileX_end - tileX_start + 1;
            xFrom = tileX_start;
            xTo = tileX_end;
        }

        int lastPointInLongitude = xTilesCount * tileGrain;
        int lastPointInLatitude = yTilesCount * tileGrain;
        int pointsInLongitudeCount = lastPointInLongitude + 1;
        int pointsInLatitudeCount = lastPointInLatitude + 1;

        int currentLongitudeOnLinePoint = 0;
        for (int tileX = xFrom; tileX <= xTo; tileX++) {
            int toTileXGrain = tileGrain + (tileX == tileX_end);
            for (int xGrain = 0; xGrain < toTileXGrain; xGrain++) {
                int currentLatitudeOnLinePoint = 0;
                float tileXGrain = tileX + (float)xGrain / tileGrain;
                float xLon = CommonUtils::tileLongitude(tileXGrain, n);
                // чтобы геометрия шла в правильном направлении (против часовой стрелки)
                xLon *= -1; // и это соответствует расчету в updateVisibleTilesSphere
                for (int tileY = tileY_start; tileY <= tileY_end; tileY++) {
                    int toTileYGrain = tileGrain + (tileY == tileY_end);
                    for (int yGrain = 0; yGrain < toTileYGrain; yGrain++) {
                        float tileYGrain = tileY + (float)yGrain / tileGrain;
                        float yLat = CommonUtils::tileLatitude(tileYGrain, n);

                        float y = radius * sin(yLat);
                        float x = radius * cos(yLat) * cos(xLon);
                        float z = radius * cos(yLat) * sin(xLon);

                        sphere_vertices.push_back(x);
                        sphere_vertices.push_back(y);
                        sphere_vertices.push_back(z);

                        float t = (float) currentLongitudeOnLinePoint / lastPointInLongitude;
                        float s = 1 - (float) currentLatitudeOnLinePoint / lastPointInLatitude;

                        if (currentLatitudeOnLinePoint == 0) {
                            s = 1;
                        }
                        if (currentLatitudeOnLinePoint == lastPointInLatitude) {
                            s = 0;
                        }
                        if (currentLongitudeOnLinePoint == 0) {
                            t = 0;
                        }
                        if (currentLongitudeOnLinePoint == lastPointInLongitude) {
                            t = 1;
                        }

                        unitSquareCoordinates.push_back(t);
                        unitSquareCoordinates.push_back(s);

                        currentLatitudeOnLinePoint++;
                    }
                }
                currentLongitudeOnLinePoint++;
            }
        }

        if (isEdge) {
            for (int tileX = xFromEdge; tileX <= xToEdge; tileX++) {
                int toTileXGrain = tileGrain + (tileX == xToEdge);
                for (int xGrain = 0; xGrain < toTileXGrain; xGrain++) {
                    int currentLatitudeOnLinePoint = 0;
                    float tileXGrain = tileX + (float)xGrain / tileGrain;
                    float xLon = CommonUtils::tileLongitude(tileXGrain, n);
                    // чтобы геометрия шла в правильном направлении (против часовой стрелки)
                    xLon *= -1; // и это соответствует расчету в updateVisibleTilesSphere
                    for (int tileY = tileY_start; tileY <= tileY_end; tileY++) {
                        int toTileYGrain = tileGrain + (tileY == tileY_end);
                        for (int yGrain = 0; yGrain < toTileYGrain; yGrain++) {
                            float tileYGrain = tileY + (float)yGrain / tileGrain;
                            float yLat = CommonUtils::tileLatitude(tileYGrain, n);

                            float y = radius * sin(yLat);
                            float x = radius * cos(yLat) * cos(xLon);
                            float z = radius * cos(yLat) * sin(xLon);

                            sphere_vertices.push_back(x);
                            sphere_vertices.push_back(y);
                            sphere_vertices.push_back(z);

                            float t = (float) currentLongitudeOnLinePoint / lastPointInLongitude;
                            float s = 1 - (float) currentLatitudeOnLinePoint / lastPointInLatitude;

                            if (currentLatitudeOnLinePoint == 0) {
                                s = 1;
                            }
                            if (currentLatitudeOnLinePoint == lastPointInLatitude) {
                                s = 0;
                            }
                            if (currentLongitudeOnLinePoint == 0) {
                                t = 0;
                            }
                            if (currentLongitudeOnLinePoint == lastPointInLongitude) {
                                t = 1;
                            }

                            unitSquareCoordinates.push_back(t);
                            unitSquareCoordinates.push_back(s);

                            currentLatitudeOnLinePoint++;
                        }
                    }
                    currentLongitudeOnLinePoint++;
                }
            }
        }

        for (int longitudePoint = 0; longitudePoint < lastPointInLongitude; longitudePoint++) {
            for (int latitudePoint = 0; latitudePoint < lastPointInLatitude; latitudePoint++) {
                int longitudeShift = pointsInLatitudeCount * longitudePoint;

                sphere_indices.push_back(longitudeShift + latitudePoint);
                sphere_indices.push_back(longitudeShift + latitudePoint + 1);
                sphere_indices.push_back(longitudeShift + latitudePoint + pointsInLatitudeCount);

                sphere_indices.push_back(longitudeShift + latitudePoint + pointsInLatitudeCount);
                sphere_indices.push_back(longitudeShift + latitudePoint + 1);
                sphere_indices.push_back(longitudeShift + latitudePoint + pointsInLatitudeCount + 1);
            }
        }

        // генерация крышек полюсов
        bool hasTopHat = tileY_start == 0;
        bool hasBottomHat = tileY_end == n - 1;
        if (hasTopHat) {
            float x = 0;
            float y = radius;
            float z = 0;
            sphere_vertices.push_back(x);
            sphere_vertices.push_back(y);
            sphere_vertices.push_back(z);
            int lastPoint = sphere_vertices.size() / 3 - 1;

            unitSquareCoordinates.push_back(1.0);
            unitSquareCoordinates.push_back(1.0);

            for (int longitudePoint = 0; longitudePoint < lastPointInLongitude; longitudePoint++) {
                int firstLonPoint = longitudePoint * pointsInLatitudeCount;
                int nextLonPoint = (longitudePoint + 1) * pointsInLatitudeCount;

                sphere_indices.push_back(lastPoint);
                sphere_indices.push_back(firstLonPoint);
                sphere_indices.push_back(nextLonPoint);
            }
        }

        if (hasBottomHat) {
            float x = 0;
            float y = -radius;
            float z = 0;
            sphere_vertices.push_back(x);
            sphere_vertices.push_back(y);
            sphere_vertices.push_back(z);
            int lastPoint = sphere_vertices.size() / 3 - 1;
            unitSquareCoordinates.push_back(0.0);
            unitSquareCoordinates.push_back(0.0);

            for (int longitudePoint = 0; longitudePoint < lastPointInLongitude; longitudePoint++) {
                int firstLonPoint = longitudePoint * pointsInLatitudeCount + pointsInLatitudeCount - 1;
                int nextLonPoint = (longitudePoint + 1) * pointsInLatitudeCount + pointsInLatitudeCount - 1;

                sphere_indices.push_back(lastPoint);
                sphere_indices.push_back(firstLonPoint);
                sphere_indices.push_back(nextLonPoint);
            }
        }
    }

    void generateFlatData(
            int tileZ_start,
            int tileZ_end,
            int tileY_start,
            int tileY_end,
            int zoom,
            int tileGrain,
            float radius
    ) {

        // clear memory of prev arrays
        std::vector<float>().swap(sphere_vertices);
        std::vector<unsigned int>().swap(sphere_indices);
        std::vector<float>().swap(normals);
        std::vector<float>().swap(unitSquareCoordinates);

        int n = pow(2, zoom);
        int tilesZCount = tileZ_end - tileZ_start + 1;
        int tilesYCount = tileY_end - tileY_start + 1;
        float tileSize = 2 * radius / n;

        for (int tileZ = tileZ_start; tileZ <= tileZ_end + 1; tileZ++) {
            for (int tileY = tileY_start; tileY <= tileY_end + 1; tileY++) {
                float x = 0.0;
                float y = -1 * (tileY * tileSize - radius);
                float z = -1 * (tileZ * tileSize - radius);

                sphere_vertices.push_back(x);
                sphere_vertices.push_back(y);
                sphere_vertices.push_back(z);

                float tileZStart = tileZ - tileZ_start;
                float tileYStart = tileY - tileY_start;
                unitSquareCoordinates.push_back((float) tileZStart / tilesZCount);
                unitSquareCoordinates.push_back(1.0 - (float) tileYStart / tilesYCount);
            }
        }

        int pointsInYCount = tileY_end - tileY_start + 2;
        LOGI("Points geometry %d", sphere_vertices.size() / 3);

        int toTileZ = tileZ_end - tileZ_start;
        int toTileY = tileY_end - tileY_start;

        for (int tileZ = 0; tileZ <= toTileZ; tileZ++) {
            for (int tileY = 0; tileY <= toTileY; tileY++) {
                int zShift = tileZ * pointsInYCount;
                sphere_indices.push_back(tileY + zShift);
                sphere_indices.push_back(tileY + 1 + zShift);
                sphere_indices.push_back(tileY + pointsInYCount + zShift);

                sphere_indices.push_back(tileY + pointsInYCount + zShift);
                sphere_indices.push_back(tileY + 1 + zShift);
                sphere_indices.push_back(tileY + pointsInYCount + 1 + zShift);
            }
        }
    }

    std::vector<float> sphere_vertices;
    std::vector<float> normals;
    std::vector<float> unitSquareCoordinates;
    std::vector<unsigned int> sphere_indices;
    std::vector<unsigned int> lineIndices;
};


#endif //TUSA_ANDROID_SPHERE_H
