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
#include "Region.h"
#include "PathTextCache.h"
#include "PointForTextPathCache.h"

class MapTile {
public:
    MapTile(int x, int y, int z);
    MapTile(int x, int y, int z, vtzero::vector_tile& tile, MapSymbols& mapSymbols);

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

    unsigned int getMaxStyleIndex() {
        return maxStyleIndex;
    }

    std::array<int,3> getTileCords() {
        return {x, y, z};
    }
    int getX() {return x; };
    int getY() {return y; };
    int getZ() {return z; };

    bool uploadedToGPU = false;

    std::unordered_map<unsigned short, MapSimpleLine> resultLinesAggregatedByStyles;
    std::unordered_map<unsigned short, MapPolygonAggregated> resultPolygonsAggregatedByStyles;
    std::unordered_map<unsigned short, MapSimplePoint> resultPointsAggregatedByStyles;
    std::unordered_map<unsigned short, MapSquarePoints> resultSquarePointsAggregatedByStyles;
    std::unordered_map<unsigned short, MapWideLine> resultWideLineAggregatedByStyles;
    std::unordered_map<uint64_t, MarkerMapTitle> resultMarkerTitles;
    std::vector<DrawTextAlongPath> resultDrawTextAlongPath;

    std::vector<float> lastPointsTest = {};
    std::vector<float> firstPointsTest = {};

    MapStyle style = MapStyle();
    void destroy();
private:
    int z;
    int x;
    int y;
    uint32_t extent;
    bool empty;
    unsigned int maxStyleIndex = 0;

    uint64_t pathIndex = 0;
    std::vector<Region> regionsShowed;

    void parseRoadTitleText(
        std::wstring useStreetName,
        std::string streetName,
        std::vector<vtzero::point>& point_array,
        MapSymbols& mapSymbols,
        uint64_t featureId
    );

    void latAndLonFromTilePoint(vtzero::point point, float& latitude, float& longitude);
};


#endif //TUSA_ANDROID_MAPTILE_H
