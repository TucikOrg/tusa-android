//
// Created by Artem on 16.09.2024.
//

#include <mapbox/earcut.hpp>
#include "MapTile.h"
#include "map/polygon_handler.h"
#include "map/linestring_handler.h"


MapTile::MapTile(int x, int y, int z, vtzero::vector_tile& tile, MapStyle& style)
        :  x(x), y(y), z(z), empty(false) {

    // Полигоны
    // unsigned short - индекс стиля
    // геометрию нужно сортировать по стилю чтобы потом рисовать все сразу
    std::map<unsigned short, unsigned int> polygonsPointsCount = {};
    std::map<unsigned short, unsigned int> polygonsIndicesCount = {};
    std::map<unsigned short, std::forward_list<MapPolygon>> polygonsResult;

    // Простые линии
    std::map<unsigned short, std::forward_list<std::vector<MapSimpleLine>>> featuresLinesResult = {};
    std::map<unsigned short, unsigned int> linesPointsCount = {};
    std::map<unsigned short, unsigned int> linesResultIndicesCount = {};

    while (auto layer = tile.next_layer()) {
        auto layerName = layer.name().to_string();

        // количестов отдельных объектов геометрии
        std::size_t feature_count = layer.num_features();
        extent = layer.extent();
        while (auto feature = layer.next_feature()) {
            auto geomType = feature.geometry_type();
            auto props = create_properties_map<layer_map_type>(feature);
            auto styleIndex = style.determineStyle(layerName, props);
            if(styleIndex == 0) {
                continue;
            }

            if(geomType == vtzero::GeomType::LINESTRING) {
                auto lineHandler = LineStringHandler();
                vtzero::decode_linestring_geometry(feature.geometry(), lineHandler);
                size_t geomSize = lineHandler.lines.size();
                std::vector<MapSimpleLine> simpleLines(geomSize);
                for (size_t geomIndex = 0; geomIndex < geomSize; ++geomIndex) {
                    const auto& point_array = lineHandler.lines[geomIndex];
                    const size_t pointsSize = point_array.size();
                    linesPointsCount[styleIndex] += pointsSize;
                    std::vector<float> vertices(pointsSize * 2);
                    for(int pointIndex = 0; pointIndex < pointsSize; ++pointIndex) {
                        auto point = point_array[pointIndex];
                        vertices[pointIndex * 2] = point.x;
                        vertices[pointIndex * 2 + 1] = -1 * point.y;
                    }

                    short amountOfInserts = 0;
                    if(pointsSize > 2) {
                        amountOfInserts = pointsSize - 2;
                    }
                    size_t indicesAmount = pointsSize + amountOfInserts;
                    std::vector<unsigned int> indices(indicesAmount);
                    for(int indicesIndex = 0; indicesIndex < indicesAmount; indicesIndex += 2) {
                        indices[indicesIndex] = indicesIndex / 2;
                        indices[indicesIndex + 1] = indicesIndex / 2 + 1;
                    }
                    linesResultIndicesCount[styleIndex] += indicesAmount;
                    simpleLines[geomIndex] = { std::move(vertices), std::move(indices) };
                }
                featuresLinesResult[styleIndex].push_front(std::move(simpleLines));

            } else if(geomType == vtzero::GeomType::POLYGON) {
                auto polygonHandler = PolygonHandler();
                vtzero::decode_polygon_geometry(feature.geometry(), polygonHandler);
                auto& polygons = polygonHandler.polygons;
                polygons.push_back(polygonHandler.polygon);
                polygonsPointsCount[styleIndex] += polygonHandler.pointsCount;
                for(std::vector<std::vector<std::array<float, 2>>> polygon : polygons) {
                    std::vector<unsigned int> indices = mapbox::earcut<unsigned int>(polygon);
                    polygonsIndicesCount[styleIndex] += indices.size();
                    polygonsResult[styleIndex].push_front(MapPolygon { polygon, indices });
                }
            }

        }
    }


    // Теперь агрегируем всю геометрию по стилям чтобы потом ее рисовать
    // агрегирование по стилю так как от стиля зависит как рисовать
    for (auto styleIndex : style.getStyles()) {

        // Простые линии
        size_t indicesShift = 0;
        size_t pointsShift = 0;
        std::vector<unsigned int> resultLinesIndices(linesResultIndicesCount[styleIndex]);
        std::vector<float> resultLinesVertices(linesPointsCount[styleIndex] * 2);
        auto& linesFeature = featuresLinesResult[styleIndex];
        for(auto& lines : linesFeature) {
            for(auto& line: lines) {
                for(int vertexIndex = 0; vertexIndex < line.vertices.size(); ++vertexIndex) {
                    auto& vertex = line.vertices[vertexIndex];
                    resultLinesVertices[vertexIndex + pointsShift * 2] = vertex;
                }
                for(int indicesIndex = 0; indicesIndex < line.indices.size(); ++indicesIndex) {
                    unsigned int indic = line.indices[indicesIndex] + pointsShift;
                    resultLinesIndices[indicesIndex + indicesShift] = indic;
                }
                indicesShift += line.indices.size();
                pointsShift += line.vertices.size() / 2;
            }
        }
        resultLinesAggregatedByStyles[styleIndex] = MapSimpleLine { std::move(resultLinesVertices), std::move(resultLinesIndices) };


        // Полигоны
        unsigned int currentPointIndex = 0;
        unsigned int currentIndicIndex = 0;
        unsigned int currentIndicShift = 0;
        std::vector<float> polygonPoints(polygonsPointsCount[styleIndex] * 2);
        std::vector<unsigned int> polygonIndices(polygonsIndicesCount[styleIndex]);

        for(auto& polygon : polygonsResult[styleIndex]) {
            auto rings = polygon.points;
            unsigned int useShiftIndices = 0;
            for(auto ring : rings) {
                useShiftIndices += ring.size();
                for(auto& point : ring) {
                    polygonPoints[currentPointIndex] = point[0];
                    polygonPoints[currentPointIndex + 1] = -1 * point[1];
                    currentPointIndex += 2;
                }
            }

            auto& indices = polygon.indices;
            for(auto indic : indices) {
                polygonIndices[currentIndicIndex++] = indic + currentIndicShift;
            }
            currentIndicShift += useShiftIndices;
        }

        resultPolygonsAggregatedByStyles[styleIndex] = MapPolygonAggregated { std::move(polygonPoints), std::move(polygonIndices) };
    }
}

MapTile::MapTile(): empty(true) {}


