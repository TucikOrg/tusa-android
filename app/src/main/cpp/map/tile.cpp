//
// Created by Artem on 08.02.2024.
//

//#include <mapbox/vector_tile.hpp>
#include "map/tile.h"
#include "util/android_log.h"
#include "util/feature_geometry_block.h"
#include "map/linestring_handler.h"
#include "boost/variant.hpp"
#include "map/polygon_handler.h"
#include <sstream>
#include <chrono>
#include <mapbox/earcut.hpp>
#include <array>
#include <vector>
#include <vector_tile.hpp>
#include <chrono>
#include <thread>


bool DEBUG_TILE_PARSE = false;

double shoelace(std::vector<std::array<float, 2>> points) {
    double leftSum = 0.0;
    double rightSum = 0.0;

    for (int i = 0; i < points.size(); ++i) {
        int j = (i + 1) % points.size();
        leftSum  += points[i][0] * points[j][1];
        rightSum += points[j][0] * points[i][1];
    }

    return leftSum - rightSum;
}

Tile::Tile(int zoom, int x, int y, vtzero::vector_tile *tile)
: zoom(zoom), x(x), y(y) {

    auto start = std::chrono::high_resolution_clock ::now();
    parseMethod1(zoom, x, y, tile);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    LOGI("Tile parse time: %f", duration.count());

    delete tile;
}

std::string Tile::makeKey(int zoom, int x, int y) {
    return std::to_string(zoom) + std::to_string(x) + std::to_string(y);
}

void Tile::decodeLines(
        size_t (&linesPointsCount)[Style::maxGeometryHeaps],
        size_t (&linesResultIndicesCount)[Style::maxGeometryHeaps],
        std::forward_list<FeatureGeometryBlock<Geometry<LineCoord, LineIndic>>> (&linesFeatureGeomBlockList)[Style::maxGeometryHeaps],
        vtzero::feature& feature,
        short styleIndex
) {
    auto lineHandler = LineStringHandler();
    vtzero::decode_linestring_geometry(feature.geometry(), lineHandler);
    size_t geomSize = lineHandler.lines.size();
    Geometry<LineCoord, LineIndic> *featureGeometry = new Geometry<LineCoord, LineIndic>[geomSize];
    for (size_t geomIndex = 0; geomIndex < geomSize; ++geomIndex) {
        const auto& point_array = lineHandler.lines[geomIndex];
        const size_t pointsSize = point_array.size();
        linesPointsCount[styleIndex] += pointsSize;
        LineCoord* lines = new LineCoord[pointsSize * 2];
        for(int pointIndex = 0; pointIndex < pointsSize; ++pointIndex) {
            auto point = point_array[pointIndex];
            lines[pointIndex * 2] = point.x;
            lines[pointIndex * 2 + 1] = -1 * point.y;
        }

        short amountOfInserts = 0;
        if(pointsSize > 2) {
            amountOfInserts = pointsSize - 2;
        }
        size_t indicesAmount = pointsSize + amountOfInserts;
        LineIndic *indices = new LineIndic[indicesAmount];
        for(int indicesIndex = 0; indicesIndex < indicesAmount; indicesIndex += 2) {
            indices[indicesIndex] = indicesIndex / 2;
            indices[indicesIndex + 1] = indicesIndex / 2 + 1;
        }
        linesResultIndicesCount[styleIndex] += indicesAmount;
        featureGeometry[geomIndex] = Geometry<LineCoord, LineIndic>(lines, pointsSize, indices, indicesAmount);
    }

    linesFeatureGeomBlockList[styleIndex].push_front(FeatureGeometryBlock<Geometry<LineCoord, LineIndic>>(featureGeometry, geomSize));
}




void Tile::parseMethod1(int zoom, int x, int y, vtzero::vector_tile *tile) {

    // жирные линии
    size_t wideLinesPointsCount[Style::maxGeometryHeaps] = {};
    size_t wideLinesResultIndicesCount[Style::maxGeometryHeaps] = {};
//    std::forward_list<FeatureGeometryBlock<Geometry<LineCoord, LineIndic>>> widePolygonedLinesFeatureGeomBlockList[Style::maxGeometryHeaps];
    std::fill(wideLinesPointsCount, wideLinesPointsCount + Style::maxGeometryHeaps, 0);
    std::fill(wideLinesResultIndicesCount, wideLinesResultIndicesCount + Style::maxGeometryHeaps, 0);

    // Полигоны
    size_t polygonsPointsCount[Style::maxGeometryHeaps] = {};
    size_t polygonsIndicesCount[Style::maxGeometryHeaps] = {};
    std::forward_list<std::pair<std::vector<std::vector<std::array<float, 2>>>, std::vector<PolygonIndic>>> polygonsFeatureGeomBlockList[Style::maxGeometryHeaps];
    std::fill(polygonsIndicesCount, polygonsIndicesCount + Style::maxGeometryHeaps, 0);
    std::fill(polygonsPointsCount, polygonsPointsCount + Style::maxGeometryHeaps, 0);


    // простые линии
    std::forward_list<FeatureGeometryBlock<Geometry<LineCoord, LineIndic>>> linesFeatureGeomBlockList[Style::maxGeometryHeaps];
    size_t linesPointsCount[Style::maxGeometryHeaps] = {};
    size_t linesResultIndicesCount[Style::maxGeometryHeaps] = {};
    std::fill(linesPointsCount, linesPointsCount + Style::maxGeometryHeaps, 0);
    std::fill(linesResultIndicesCount, linesResultIndicesCount + Style::maxGeometryHeaps, 0);


    while (auto layer = tile->next_layer()) {
        auto name = layer.name().to_string();
        std::size_t feature_count = layer.num_features();
        uint32_t extent = layer.extent();
        while (auto feature = layer.next_feature()) {
            auto geomType = feature.geometry_type();
            auto props = create_properties_map<layer_map_type>(feature);
            auto styleIndex = style.determineStyle(name, props);
            if(styleIndex == -1) {
                continue;
            }

            if(geomType == vtzero::GeomType::LINESTRING) {
                float useLineTriangleStrip = style.getTrianglesStripLineWidthOfHeap(styleIndex);
                if (useLineTriangleStrip != -1) {
                    // это широкие линиии
                    // их нужно рисовать как полигоны потому что open gl es не поддерживает тостые линии
                    decodeWideLines(wideLinesPointsCount,
                                    wideLinesResultIndicesCount,
                                    feature,
                                    styleIndex,
                                    widePolygonedLinesFeatureGeomBlockList,
                                    useLineTriangleStrip
                                    );
                } else {
                    // Это обычные линии
                    decodeLines(linesPointsCount, linesResultIndicesCount, linesFeatureGeomBlockList, feature, styleIndex);
                }
            } else if(geomType == vtzero::GeomType::POLYGON) {
                auto polygonHandler = PolygonHandler();
                vtzero::decode_polygon_geometry(feature.geometry(), polygonHandler);
                auto& polygons = polygonHandler.polygons;
                polygons.push_back(polygonHandler.polygon);
                polygonsPointsCount[styleIndex] += polygonHandler.pointsCount;
                for(std::vector<std::vector<std::array<float, 2>>>& polygon : polygons) {
                    std::vector<PolygonIndic> indices = mapbox::earcut<PolygonIndic>(polygon);
                    polygonsIndicesCount[styleIndex] += indices.size();
                    polygonsFeatureGeomBlockList[styleIndex].push_front({polygon, indices});
                }
            }
        }
    }


    // Проходимся по стилям и создаем результирующие суммы координат
    for(short useStyleIndex = 0; useStyleIndex < Style::maxGeometryHeaps; ++useStyleIndex) {

        // простые линии
        size_t indicesShift = 0;
        size_t pointsShift = 0;
        LineIndic *resultLinesIndices = new LineIndic[linesResultIndicesCount[useStyleIndex]];
        LineCoord *resultLinesPoints = new LineCoord[linesPointsCount[useStyleIndex] * 2];
        auto& listOfGeometries = linesFeatureGeomBlockList[useStyleIndex];
        for(auto& arrayOfGeometries : listOfGeometries) {
            for(int useGeometry = 0; useGeometry < arrayOfGeometries.size; ++useGeometry) {
                auto& geometry = arrayOfGeometries.data[useGeometry];
                for(int pointIndex = 0; pointIndex < geometry.pointsCount * 2; ++pointIndex) {
                    auto& coordinate = geometry.points[pointIndex];
                    resultLinesPoints[pointIndex + pointsShift * 2] = coordinate;
                }
                for(int indicesIndex = 0; indicesIndex < geometry.indicesCount; ++indicesIndex) {
                    LineIndic in = geometry.indices[indicesIndex] + pointsShift;
                    resultLinesIndices[indicesIndex + indicesShift] = in;
                }
                indicesShift += geometry.indicesCount;
                pointsShift += geometry.pointsCount;
            }
        }
        resultLines[useStyleIndex] = Geometry<LineCoord, LineIndic>(
                resultLinesPoints, linesPointsCount[useStyleIndex],
                resultLinesIndices, linesResultIndicesCount[useStyleIndex]
        );

        // жирные линии
//        makeWidthLinesResult(
//            wideLinesPointsCount,
//            wideLinesResultIndicesCount,
//            useStyleIndex,
//            widePolygonedLinesFeatureGeomBlockList
//        );


        if(DEBUG_TILE_PARSE) {
            std::stringstream ss;
            ss << "End geometry: ";
            for(int i = 0; i < linesPointsCount[useStyleIndex] * 2; ++i) {
                ss << i / 2 << ":" << resultLinesPoints[i] << ", ";
            }
            ss << "End indices: ";
            for(int i = 0; i < linesResultIndicesCount[useStyleIndex]; ++i) {
                ss << resultLinesIndices[i] << " ";
            }
            std::string resultInfo = ss.str();
            LOGI("%s", resultInfo.data());
        }

        PolygonIndic currentPointIndex = 0;
        PolygonIndic currentIndicIndex = 0;
        PolygonIndic currentIndicShift = 0;
        float *resultfloats = new float[polygonsPointsCount[useStyleIndex] * 2];
        PolygonIndic *resultPolygonIndices = new PolygonIndic[polygonsIndicesCount[useStyleIndex]];
        for(auto& polygonPair : polygonsFeatureGeomBlockList[useStyleIndex]) {
            auto rings = polygonPair.first;
            PolygonIndic useShiftIndices = 0;
            for(auto ring : rings) {
                useShiftIndices += ring.size();
                for(auto& point : ring) {
                    resultfloats[currentPointIndex] = point[0];
                    resultfloats[currentPointIndex + 1] = -1 * point[1];
                    currentPointIndex += 2;
                }
            }

            std::vector<PolygonIndic> indices = polygonPair.second;
            for(auto indic : indices) {
                resultPolygonIndices[currentIndicIndex++] = indic + currentIndicShift;
            }
            currentIndicShift += useShiftIndices;
        }


        if(DEBUG_TILE_PARSE) {
            std::stringstream ss;
            ss << "Polygon End geometry: ";
            for(int i = 0; i < polygonsPointsCount[useStyleIndex] * 2; ++i) {
                ss << i / 2 << ":" << resultfloats[i] << ", ";
            }
            ss << "Polygon End indices: ";
            for(int i = 0; i < polygonsIndicesCount[useStyleIndex]; ++i) {
                ss << resultPolygonIndices[i] << " ";
            }
            std::string resultInfo = ss.str();
            LOGI("%s", resultInfo.data());
        }

        resultPolygons[useStyleIndex] = Geometry<float, PolygonIndic>(
                resultfloats, polygonsPointsCount[useStyleIndex],
                resultPolygonIndices, polygonsIndicesCount[useStyleIndex]
        );
    }
}

void Tile::decodeWideLines(
        size_t (&wideLinesPointsCount)[Style::maxGeometryHeaps],
        size_t (&wideLinesResultIndicesCount)[Style::maxGeometryHeaps],
        vtzero::feature& feature,
        short styleIndex,
        std::forward_list<FeatureGeometryBlock<Geometry<LineCoord, LineIndic>>> (&widePolygonedLinesFeatureGeomBlockList)[Style::maxGeometryHeaps],
        float width
) {
    auto lineHandler = LineStringHandler();
    vtzero::decode_linestring_geometry(feature.geometry(), lineHandler);
    size_t geomSize = lineHandler.lines.size();
    Geometry<LineCoord, LineIndic> *featureGeometry = new Geometry<LineCoord, LineIndic>[geomSize];
    for (size_t geomIndex = 0; geomIndex < geomSize; ++geomIndex) {
        const auto& point_array = lineHandler.lines[geomIndex];
        const size_t pointsSize = point_array.size();
        const size_t widePointsSize = pointsSize * 2;
        wideLinesPointsCount[styleIndex] += widePointsSize;
        LineCoord* lines = new LineCoord[widePointsSize * 2];
        float nx;
        float ny;
        for(int pointIndex = 0; pointIndex < pointsSize; ++pointIndex) {
            auto point = point_array[pointIndex];
            if (pointIndex + 1 < pointsSize) {
                auto nextPoint = point_array[pointIndex + 1];
                float dx = nextPoint.x - point.x;
                float dy = nextPoint.y - point.y;
                nx = -dy;
                ny = dx;
                float len = sqrt(pow(nx, 2.0) + pow(ny, 2.0));
                nx = nx / len;
                ny = ny / len;
            }

            float x1 = point.x + nx * width;
            float y1 = point.y + ny * width;
            float x2 = point.x - nx * width;
            float y2 = point.y - ny * width;

            lines[pointIndex * 4] = x1;
            lines[pointIndex * 4 + 1] = -y1;
            lines[pointIndex * 4 + 2] = x2;
            lines[pointIndex * 4 + 3] = -y2;
        }

        short amountOfInserts = 0;
        if(pointsSize > 2) {
            amountOfInserts = pointsSize - 2;
        }
        size_t indicesAmount = pointsSize + amountOfInserts;
        LineIndic *indices = new LineIndic[indicesAmount];
        for(int indicesIndex = 0; indicesIndex < indicesAmount; indicesIndex += 2) {
            indices[indicesIndex] = indicesIndex / 2;
            indices[indicesIndex + 1] = indicesIndex / 2 + 1;
        }
        wideLinesResultIndicesCount[styleIndex] += indicesAmount;
        featureGeometry[geomIndex] = Geometry<LineCoord, LineIndic>(lines, widePointsSize, indices, indicesAmount);
    }

    widePolygonedLinesFeatureGeomBlockList[styleIndex].push_front(FeatureGeometryBlock<Geometry<LineCoord, LineIndic>>(featureGeometry, geomSize));
}

void Tile::makeWidthLinesResult(
        size_t (&wideLinesPointsCount)[Style::maxGeometryHeaps],
        size_t (&wideLinesResultIndicesCount)[Style::maxGeometryHeaps],
        short useStyleIndex,
        std::forward_list<FeatureGeometryBlock<Geometry<LineCoord, LineIndic>>> (&widePolygonedLinesFeatureGeomBlockList)[Style::maxGeometryHeaps]
) {
    size_t indicesShift = 0;
    size_t pointsShift = 0;
    LineIndic *resultLinesIndices = new LineIndic[wideLinesResultIndicesCount[useStyleIndex]];
    LineCoord *resultLinesPoints = new LineCoord[wideLinesPointsCount[useStyleIndex] * 2];
    auto& listOfGeometries = widePolygonedLinesFeatureGeomBlockList[useStyleIndex];
    for(auto& arrayOfGeometries : listOfGeometries) {
        for(int useGeometry = 0; useGeometry < arrayOfGeometries.size; ++useGeometry) {
            auto& geometry = arrayOfGeometries.data[useGeometry];
            for(int pointIndex = 0; pointIndex < geometry.pointsCount * 2; ++pointIndex) {
                auto& coordinate = geometry.points[pointIndex];
                resultLinesPoints[pointIndex + pointsShift * 2] = coordinate;
            }
            for(int indicesIndex = 0; indicesIndex < geometry.indicesCount; ++indicesIndex) {
                LineIndic in = geometry.indices[indicesIndex] + pointsShift;
                resultLinesIndices[indicesIndex + indicesShift] = in;
            }
            indicesShift += geometry.indicesCount;
            pointsShift += geometry.pointsCount;
        }
    }

    resultWideLines[useStyleIndex] = Geometry<LineCoord, LineIndic>(
            resultLinesPoints, wideLinesPointsCount[useStyleIndex],
            resultLinesIndices, wideLinesResultIndicesCount[useStyleIndex]
    );
}
