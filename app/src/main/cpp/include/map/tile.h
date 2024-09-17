//
// Created by Artem on 08.02.2024.
//

#ifndef TUSA_TILE_H
#define TUSA_TILE_H

#include <string>
#include <boost/variant.hpp>
#include "geometry/geometry.h"
#include <forward_list>
#include <vector_tile.hpp>
#include "util/feature_geometry_block.h"
#include "style/style.h"
#include "renderer/map_draw_call.h"

using PolygonIndic = unsigned int;


using LineCoord = float;
using LineIndic = unsigned int;

class Tile {
public:
    Tile(int zoom, int x, int y, vtzero::vector_tile *tile);

    static std::string makeKey(int zoom, int x, int y);
    int getZ() {return zoom;}
    int getX() {return x;}
    int getY() {return y;}

    Style style = Style();
    Geometry<LineCoord, LineIndic> *resultLines = new Geometry<LineCoord, LineIndic>[Style::maxGeometryHeaps];
    Geometry<float, PolygonIndic> *resultPolygons = new Geometry<float, PolygonIndic>[Style::maxGeometryHeaps];
    Geometry<LineCoord, LineIndic> *resultWideLines = new Geometry<float, PolygonIndic>[Style::maxGeometryHeaps];

    std::forward_list<FeatureGeometryBlock<Geometry<LineCoord, LineIndic>>> widePolygonedLinesFeatureGeomBlockList[Style::maxGeometryHeaps];

    std::vector<MapDrawCall> drawCalls = {};

    ~Tile() {
        delete[] resultLines;
        delete[] resultPolygons;
    }
private:

    int zoom;
    int x;
    int y;

    void parseMethod1(int zoom, int x, int y, vtzero::vector_tile *tile);
    void decodeWideLines(
            size_t (&wideLinesPointsCount)[Style::maxGeometryHeaps],
            size_t (&wideLinesResultIndicesCount)[Style::maxGeometryHeaps],
            vtzero::feature& feature,
            short styleIndex,
            std::forward_list<FeatureGeometryBlock<Geometry<LineCoord, LineIndic>>> (&widePolygonedLinesFeatureGeomBlockList)[Style::maxGeometryHeaps],
            float width
    );
    void decodeLines(size_t (&linesPointsCount)[Style::maxGeometryHeaps], size_t (&linesResultIndicesCount)[Style::maxGeometryHeaps], std::forward_list<FeatureGeometryBlock<Geometry<LineCoord, LineIndic>>> (&linesFeatureGeomBlockList)[Style::maxGeometryHeaps], vtzero::feature& feature, short styleIndex);

    void makeWidthLinesResult(
            size_t (&wideLinesPointsCount)[Style::maxGeometryHeaps],
            size_t (&wideLinesResultIndicesCount)[Style::maxGeometryHeaps],
            short useStyleIndex,
            std::forward_list<FeatureGeometryBlock<Geometry<LineCoord, LineIndic>>> (&widePolygonedLinesFeatureGeomBlockList)[Style::maxGeometryHeaps]
    );
};


#endif //TUSA_TILE_H
