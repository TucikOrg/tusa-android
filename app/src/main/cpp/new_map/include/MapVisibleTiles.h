//
// Created by Artem on 19.09.2024.
//

#ifndef TUSA_ANDROID_MAPVISIBLETILES_H
#define TUSA_ANDROID_MAPVISIBLETILES_H

#include "MapControls.h"
#include "MapTest.h"
#include <vector>
#include <array>

#include <Eigen/Dense>

class MapVisibleTiles {
public:
    std::array<std::array<float, 2>, 2> getPlaneCorners(
            Eigen::Matrix4f pv,
            float planeZ
    );

    std::array<float, 2> getYTilesVisibleEdges(int tilesZoom, float planeWidth, MapControls &mapControls, Eigen::Matrix4f pvForPlane);

    std::array<float, 2> getXTilesVisibleEdges(float shiftX) {
        // от границы до границы
        // 1 - 3 означает 1 и 2 тайл
        // 3 тайл это конец видимой области
        float xStart = shiftX;
        float xEnd = xStart + 2;
        return { xStart, xEnd };
    }

    std::vector<int> getVisibleXTiles(std::array<float, 2> visibleXEdges, int tilesZoom) {
        int n = pow(2, tilesZoom);
        std::vector<int> xTiles;
        float fromXTile = visibleXEdges[0];
        float toXTile = visibleXEdges[1];
        for (float xt = fromXTile; (int) xt < toXTile; xt++) {
            float xCord = fmod(xt, n);
            xTiles.push_back(xCord);
        }
        return std::move(xTiles);
    }

    std::vector<int> getVisibleYTiles(std::array<float, 2> visibleYEdges, int tilesZoom) {
        int n = pow(2, tilesZoom);
        std::vector<int> yTiles;
        float fromYTile = visibleYEdges[0];
        float toYTile = visibleYEdges[1];
        for (int yTile = fromYTile; yTile < toYTile; yTile++) {
            yTiles.push_back(yTile);
        }
        return yTiles;
    }

private:

    std::array<float, 2>  getPlanePoint(
            Eigen::Vector4f firstPlane,
            Eigen::Vector4f secondPlane,
            float planeZ
    );
};


#endif //TUSA_ANDROID_MAPVISIBLETILES_H
