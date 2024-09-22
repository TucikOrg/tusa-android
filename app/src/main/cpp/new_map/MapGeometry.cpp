//
// Created by Artem on 16.09.2024.
//

#include "MapGeometry.h"
#include <vector>

std::vector<float> MapGeometry::generatePlainGeometry(
        std::vector<float>& uv,
        std::vector<float>& planetUV,
        std::vector<unsigned int>& indices,
        std::vector<int> visibleY,
        int z,
        float planeWidth,
        int lod,
        float planeZ
) {
    float halfWidth         = planeWidth / 2.0;
    int xTilesAmount        = 2;
    int n                   = pow(2, z);
    float uTileDelta        = 1.0 / n;
    float uTileLodDelta     = uTileDelta / lod;
    float tileSize          = planeWidth / n;
    int yTilesAmount        = visibleY.size();
    float lodDelta          = tileSize / lod;

    if (n == 1) {
        xTilesAmount = 1;
    }

    uv.clear();
    indices.clear();

    int yStart = visibleY[0];
    int yEnd = visibleY[visibleY.size() - 1];

    int yPointsAmount   = yTilesAmount * lod + 1;
    int xPointsAmount   = xTilesAmount * lod + 1;
    int xPointLastIndex = xPointsAmount - 1;
    int yPointLastIndex = yPointsAmount - 1;


    float uDeltaXTilesSize    = xTilesAmount * uTileDelta;
    float planetStartU          = 0.5 - uDeltaXTilesSize / 2.0;
    int xiEnd                 = xTilesAmount - 1;
    float xShift              = -xTilesAmount * tileSize / 2.0;
    int xPointIndex           = 0;

    std::vector<float> vertices;
    for (int xi = 0; xi <= xiEnd; xi++) {
        int toXLod = lod + (xi == xiEnd);
        for (int xLod = 0; xLod < toXLod; xLod++) {
            float x = tileSize * xi + xLod * lodDelta + xShift;
            int yPointIndex = 0;
            for (int yi = yStart; yi <= yEnd; yi++) {
                int toYLod = lod + (yi == yEnd);

                for (int yLod = 0; yLod < toYLod; yLod++) {
                    float y = halfWidth - yi * tileSize - yLod * lodDelta;

                    vertices.push_back(x);
                    vertices.push_back(y);
                    vertices.push_back(planeZ);
                    uv.push_back((float)xPointIndex / xPointLastIndex);
                    uv.push_back(1.0 - ((float)yPointIndex / yPointLastIndex));

                    planetUV.push_back(planetStartU + uTileLodDelta * xPointIndex);
                    float currentY = yi + (float) yLod / lod;
                    planetUV.push_back(currentY / n);

                    yPointIndex++;
                }
            }
            xPointIndex++;
        }
    }

    int xIndicesEnd = xTilesAmount * lod;
    for (int x = 0; x < xIndicesEnd; x++) {
        for (int y = 0; y < yPointsAmount - 1; y++) {
            int i = x * yPointsAmount + y;
            indices.push_back(i);
            indices.push_back(i + 1);
            indices.push_back(i + yPointsAmount);
            indices.push_back(i + 1);
            indices.push_back(i + yPointsAmount);
            indices.push_back(i + yPointsAmount + 1);
        }
    }

    return std::move(vertices);
}
