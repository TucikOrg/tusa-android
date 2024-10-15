//
// Created by Artem on 16.09.2024.
//

#include <mapbox/earcut.hpp>
#include "MapTile.h"
#include "map/polygon_handler.h"
#include "map/linestring_handler.h"
#include "MapWideLine.h"
#include "util/android_log.h"

MapTile::MapTile(int x, int y, int z): empty(true), x(x), y(y), z(z) {}

MapTile::MapTile(int x, int y, int z, vtzero::vector_tile& tile)
        :  x(x), y(y), z(z), empty(false) {

    // Полигоны
    // unsigned short - индекс стиля
    // геометрию нужно сортировать по стилю чтобы потом рисовать все сразу
    std::map<unsigned short, unsigned int> polygonsPointsCount = {};
    std::map<unsigned short, unsigned int> polygonsIndicesCount = {};
    std::map<unsigned short, std::forward_list<MapPolygon>> polygonsResult;

    // Простые точки
    std::map<unsigned short, std::forward_list<std::vector<float>>> simplePoints = {};
    std::map<unsigned short, unsigned int> simplePointsVerticesCount = {};

    // Квадратные точки
    std::map<unsigned short, std::forward_list<std::vector<float>>> squarePoints = {};
    std::map<unsigned short, unsigned int> squarePointsCount = {};

    // Простые линии
    std::map<unsigned short, std::forward_list<std::vector<MapSimpleLine>>> featuresLinesResult = {};
    std::map<unsigned short, unsigned int> linesPointsCount = {};
    std::map<unsigned short, unsigned int> linesResultIndicesCount = {};

    // Широкие линии
    std::map<unsigned short, std::forward_list<std::vector<MapWideLine>>> featuresWideLinesResult = {};
    std::map<unsigned short, unsigned int> wideLinesPointsCount = {};
    std::map<unsigned short, unsigned int> wideLinesResultIndicesCount = {};

    while (auto layer = tile.next_layer()) {
        auto layerName = layer.name().to_string();

        // количестов отдельных объектов геометрии
        std::size_t feature_count = layer.num_features();
        extent = layer.extent();
        while (auto feature = layer.next_feature()) {
            auto geomType = feature.geometry_type();
            auto props = create_properties_map<layer_map_type>(feature);
            auto styleIndex = style.determineStyle(layerName, props, z);
            if(styleIndex == 0) {
                continue;
            }

            if(geomType == vtzero::GeomType::LINESTRING) {
                auto lineHandler = LineStringHandler();
                vtzero::decode_linestring_geometry(feature.geometry(), lineHandler);
                size_t geomSize = lineHandler.lines.size();
                bool isWideLine = style.getIsWideLine(styleIndex);
                if (isWideLine) {
                    // широкая линия
                    float width = style.getLineWidth(styleIndex);
                    float h = width / 2;
                    std::vector<MapWideLine> wideLines(geomSize);

                    float nx = 0;
                    float ny = 0;
                    std::vector<float> verticesSquarePoints(geomSize * 2 * 4 * 2);
                    squarePointsCount[styleIndex] += geomSize * 2;
                    for (size_t geomIndex = 0; geomIndex < geomSize; ++geomIndex) {
                        const auto& point_array = lineHandler.lines[geomIndex];
                        const size_t pointsSize = point_array.size();
                        const size_t wideLinePointsSize = pointsSize * 2;
                        wideLinesPointsCount[styleIndex] += wideLinePointsSize;
                        std::vector<float> vertices(wideLinePointsSize * 2);
                        std::vector<float> uv(wideLinePointsSize * 2);
                        std::vector<float> perpendiculars(wideLinePointsSize * 2);
                        std::vector<float> pointsVertices(wideLinePointsSize);
                        std::vector<float> shiftVector(wideLinePointsSize);
                        std::vector<float> color = CommonUtils::toOpenGlColor(CSSColorParser::parse(Utils::generateRandomColor()));
                        for(int pointIndex = 0; pointIndex < pointsSize; ++pointIndex) {
                            auto point = point_array[pointIndex];
                            auto isLastPoint = pointIndex == pointsSize - 1;
                            if (pointIndex == 0 || isLastPoint) {
                                unsigned int add = isLastPoint * 8;
                                unsigned int startFrom = geomIndex * 2 * 4 * 2;
                                // square point to draw line endings
                                verticesSquarePoints[startFrom + 0 + add] = point.x;
                                verticesSquarePoints[startFrom+ 1 + add] = -point.y;
                                verticesSquarePoints[startFrom + 2 + add] = point.x;
                                verticesSquarePoints[startFrom + 3 + add] = -point.y;
                                verticesSquarePoints[startFrom + 4 + add] = point.x;
                                verticesSquarePoints[startFrom + 5 + add] = -point.y;
                                verticesSquarePoints[startFrom + 6 + add] = point.x;
                                verticesSquarePoints[startFrom + 7 + add] = -point.y;
                            }

                            if (pointIndex + 1 < pointsSize) {
                                auto nextPoint = point_array[pointIndex + 1];
                                float dx = nextPoint.x - point.x;
                                float dy = point.y - nextPoint.y;
                                nx = -dy;
                                ny = dx;
                                float len = sqrt(pow(nx, 2.0) + pow(ny, 2.0));
                                nx = nx / len;
                                ny = ny / len;
                            }

                            pointsVertices[pointIndex * 2 + 0] = point.x;
                            pointsVertices[pointIndex * 2 + 1] = -point.y;

                            vertices[pointIndex * 4 + 0] = point.x;
                            vertices[pointIndex * 4 + 1] = -point.y;
                            vertices[pointIndex * 4 + 2] = point.x;
                            vertices[pointIndex * 4 + 3] = -point.y;

                            uv[pointIndex * 4 + 0] = 0;
                            uv[pointIndex * 4 + 1] = 0;
                            uv[pointIndex * 4 + 2] = 1;
                            uv[pointIndex * 4 + 3] = 0;

                            perpendiculars[pointIndex * 4 + 0] = nx;
                            perpendiculars[pointIndex * 4 + 1] = ny;
                            perpendiculars[pointIndex * 4 + 2] = -nx;
                            perpendiculars[pointIndex * 4 + 3] = -ny;
                        }

                        size_t indicesAmount = (pointsSize - 1) * 6;
                        std::vector<unsigned int> indices(indicesAmount);
                        for(int i = 0; i < pointsSize - 1; i++) {
                            unsigned int pointNum = i * 2;
                            unsigned int startI = i * 6;
                            indices[startI + 0] = 0 + pointNum;
                            indices[startI + 1] = 1 + pointNum;
                            indices[startI + 2] = 2 + pointNum;

                            indices[startI + 3] = 1 + pointNum;
                            indices[startI + 4] = 3 + pointNum;
                            indices[startI + 5] = 2 + pointNum;
                        }

                        wideLinesResultIndicesCount[styleIndex] += indicesAmount;
                        wideLines[geomIndex] = {
                             std::move(vertices),
                             std::move(indices),
                             std::move(perpendiculars),
                             std::move(uv)
                        };
                    }

                    squarePoints[styleIndex].push_front(std::move(verticesSquarePoints));
                    featuresWideLinesResult[styleIndex].push_front(std::move(wideLines));
                }

                // простая линия
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
                    if (vertices.size() == 0) {
                        int i = 2;
                    }
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

        // Простые точки
        unsigned int simplePointsVerticesCountOfStyle = simplePointsVerticesCount[styleIndex];
        std::vector<float> simplePointsVertices(simplePointsVerticesCountOfStyle);
        int simplePointCopyIndex = 0;
        for (auto& points : simplePoints[styleIndex]) {
            for (int i = 0; i < points.size(); ++i) {
                simplePointsVertices[simplePointCopyIndex] = points[i];
                simplePointCopyIndex++;
            }
        }
        resultPointsAggregatedByStyles[styleIndex] = MapSimplePoint { std::move(simplePointsVertices) };

        // Квадратные точки
        unsigned int squarePointsCountOfStyle = squarePointsCount[styleIndex];
        std::vector<float> squarePointVertices(squarePointsCountOfStyle * 4 * 2);
        unsigned int squarePointCopyIndex = 0;
        for (auto& squarePoint : squarePoints[styleIndex]) {
            for (auto vertex : squarePoint) {
                squarePointVertices[squarePointCopyIndex] = vertex;
                squarePointCopyIndex++;
            }
        }
        std::vector<unsigned int> squarePointIndices(squarePointsCountOfStyle * 6);
        for (int i = 0; i < squarePointsCountOfStyle; i++) {
            int startPoint = i * 4;
            squarePointIndices[i * 6 + 0] = 0 + startPoint;
            squarePointIndices[i * 6 + 1] = 1 + startPoint;
            squarePointIndices[i * 6 + 2] = 2 + startPoint;
            squarePointIndices[i * 6 + 3] = 2 + startPoint;
            squarePointIndices[i * 6 + 4] = 3 + startPoint;
            squarePointIndices[i * 6 + 5] = 0 + startPoint;
        }
        std::vector<float> squarePointShift(squarePointsCountOfStyle * 4 * 2);
        for (int i = 0; i < squarePointsCountOfStyle; i++) {
            int skip = 4 * 2 * i;
            squarePointShift[skip + 0] = -1.0f;
            squarePointShift[skip + 1] = -1.0f;
            squarePointShift[skip + 2] = 1.0f;
            squarePointShift[skip + 3] = -1.0f;
            squarePointShift[skip + 4] = 1.0f;
            squarePointShift[skip + 5] = 1.0f;
            squarePointShift[skip + 6] = -1.0f;
            squarePointShift[skip + 7] = 1.0f;
        }
        std::vector<float> squarePointUVs(squarePointsCountOfStyle * 4 * 2);
        for (int i = 0; i < squarePointsCountOfStyle; i++) {
            int skip = 4 * 2 * i;
            squarePointUVs[skip + 0] = 0.0f;
            squarePointUVs[skip+ 1] = 0.0f;
            squarePointUVs[skip + 2] = 1.0f;
            squarePointUVs[skip + 3] = 0.0f;
            squarePointUVs[skip + 4] = 1.0f;
            squarePointUVs[skip + 5] = 1.0f;
            squarePointUVs[skip + 6] = 0.0f;
            squarePointUVs[skip + 7] = 1.0f;
        }
        resultSquarePointsAggregatedByStyles[styleIndex] = MapSquarePoints {
            std::move(squarePointVertices),
            std::move(squarePointUVs),
            std::move(squarePointIndices),
            std::move(squarePointShift)
        };

        // Широкие линии
        std::vector<unsigned int> wideLinesIndices(wideLinesResultIndicesCount[styleIndex]);
        std::vector<float> wideLinesVertices(wideLinesPointsCount[styleIndex] * 2);
        std::vector<float> wideLinesUVs(wideLinesPointsCount[styleIndex] * 2);
        std::vector<float> wideLinesPerpendiculars(wideLinesPointsCount[styleIndex] * 2);
        unsigned int wideLineCopyIndex = 0;
        unsigned int wideLineIndicesCopyIndex = 0;
        unsigned int indicShift = 0;
        for (auto& wideLines : featuresWideLinesResult[styleIndex]) {
            for (auto& wideLine : wideLines) {
                for (int i = 0; i < wideLine.vertices.size(); i++) {
                    wideLinesVertices[wideLineCopyIndex] = wideLine.vertices[i];
                    wideLinesUVs[wideLineCopyIndex] = wideLine.uv[i];
                    wideLinesPerpendiculars[wideLineCopyIndex] = wideLine.perpendiculars[i];
                    wideLineCopyIndex++;
                }
                for (auto& indic : wideLine.indices) {
                    wideLinesIndices[wideLineIndicesCopyIndex] = indic + indicShift;
                    wideLineIndicesCopyIndex++;
                }
                indicShift += wideLine.vertices.size() / 2;
            }
        }
        resultWideLineAggregatedByStyles[styleIndex] = MapWideLine {
            std::move(wideLinesVertices),
            std::move(wideLinesIndices),
            std::move(wideLinesPerpendiculars),
            std::move(wideLinesUVs)
        };

    }
}

bool MapTile::cover(std::array<int, 3> otherTile) {
    if (z > otherTile[2]) {
        return false;
    }

    // Коэффициент масштаба между уровнями зума
    int scale = 1 << (otherTile[2] - z);

    // Преобразуем координаты тайла1 на уровень зума тайла2
    int transformed_x1 = x * scale;
    int transformed_y1 = y * scale;

    // Проверяем, покрывает ли тайл1 тайл2
    return (transformed_x1 <= otherTile[0] && otherTile[0] < transformed_x1 + scale) &&
           (transformed_y1 <= otherTile[1] && otherTile[1] < transformed_y1 + scale);

}

bool MapTile::coverOneOther(int x1, int y1, int z1, int x2, int y2, int z2) {
    if (z1 > z2) {
        return false;
    }

    // Коэффициент масштаба между уровнями зума
    int scale = 1 << (z2 - z1);

    // Преобразуем координаты тайла1 на уровень зума тайла2
    int transformed_x1 = x1 * scale;
    int transformed_y1 = y1 * scale;

    // Проверяем, покрывает ли тайл1 тайл2
    return (transformed_x1 <= x2 && x2 < transformed_x1 + scale) &&
           (transformed_y1 <= y2 && y2 < transformed_y1 + scale);
}




