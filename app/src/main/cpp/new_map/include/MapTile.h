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
#include "MapPolygonAggregated.h"
#include "style/style.h"


class MapTile {
public:
    MapTile();
    MapTile(int x, int y, int z, vtzero::vector_tile& tile, MapStyle& style);

    static std::string makeKey(int x, int y, int z) {
        return std::to_string(x) + std::to_string(y) + std::to_string(z);
    }

    bool isEmpty() {
        return empty;
    }

    std::string key() {
        return makeKey(x, y, z);
    }

    uint32_t getExtent() {
        return extent;
    }

    std::map<unsigned short, MapSimpleLine> resultLinesAggregatedByStyles;
    std::map<unsigned short, MapPolygonAggregated> resultPolygonsAggregatedByStyles;
private:
    int z;
    int x;
    int y;
    uint32_t extent;
    bool empty;
};


#endif //TUSA_ANDROID_MAPTILE_H
