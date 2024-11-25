//
// Created by Artem on 16.09.2024.
//

#ifndef TUSA_ANDROID_MAPTILE_H
#define TUSA_ANDROID_MAPTILE_H

#include <vector_tile.hpp>
#include <forward_list>
#include <map>
#include "MapPolygon.h"
#include "MapStyle.h"
#include "MapSimpleLine.h"
#include "MapWideLine.h"
#include "MapSimplePoint.h"
#include "MapSquarePoints.h"
#include "MapPolygonAggregated.h"
#include "MarkerMapTitle.h"
#include "DrawTextAlongPath.h"

class MapTile {
public:
    MapTile(int x, int y, int z);
    MapTile(int x, int y, int z, vtzero::vector_tile& tile);

    bool cover(std::array<int, 3> otherTile);

    static bool coverOneOther(int x1, int y1, int z1, int x2, int y2, int z2);

    static uint64_t makeKey(int x, int y, int z) {
        return (static_cast<uint64_t>(x) << 32) | (static_cast<uint32_t>(y) << 16) | static_cast<uint16_t>(z);
    }

    bool isRoot() {
        return x == 0 && y == 0 && z == 0;
    }

    bool is(std::array<int, 3> tileCords) {
        return x == tileCords[0] && y == tileCords[1] && z == tileCords[3];
    }

    bool isEmpty() {
        return empty;
    }

    uint32_t getExtent() {
        return extent;
    }

    unsigned int getLastStyle() {
        return lastStyle;
    }

    std::array<int,3> getTileCords() {
        return {x, y, z};
    }
    int getX() {return x; };
    int getY() {return y; };
    int getZ() {return z; };

    bool uploadedToGPU = false;
    std::map<unsigned short, MapSimpleLine> resultLinesAggregatedByStyles;
    std::map<unsigned short, MapPolygonAggregated> resultPolygonsAggregatedByStyles;
    std::map<unsigned short, MapSimplePoint> resultPointsAggregatedByStyles;
    std::map<unsigned short, MapSquarePoints> resultSquarePointsAggregatedByStyles;
    std::map<unsigned short, MapWideLine> resultWideLineAggregatedByStyles;
    std::vector<MarkerMapTitle> resultMarkerTitles;
    std::unordered_map<uint64_t, DrawTextAlongPath> resultDrawTextAlongPath;

    std::vector<float> lastPointsTest = {};
    std::vector<float> firstPointsTest = {};

    MapStyle style = MapStyle();
private:
    unsigned short lastStyle = 0;
    int z;
    int x;
    int y;
    uint32_t extent;
    bool empty;
};


#endif //TUSA_ANDROID_MAPTILE_H
