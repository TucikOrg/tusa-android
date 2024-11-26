//
// Created by Artem on 16.09.2024.
//

#include <mapbox/earcut.hpp>
#include <types.hpp>
#include "MapTile.h"
#include "map/polygon_handler.h"
#include "map/linestring_handler.h"
#include "MapWideLine.h"
#include "util/android_log.h"
#include "PointHandler.h"

MapTile::MapTile(int x, int y, int z): empty(true), x(x), y(y), z(z) {}

MapTile::MapTile(int x, int y, int z, vtzero::vector_tile& tile, MapSymbols& mapSymbols)
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

        // Количестов отдельных объектов геометрии
        std::size_t feature_count = layer.num_features();
        extent = layer.extent();
        while (auto feature = layer.next_feature()) {
            while (auto property = feature.next_property()) {
                if (property.key() == "filterrank") {
                    auto type = property.value().type();
                }
                if (property.key() == "symbolrank") {
                    auto type = property.value().type();
                }

//                if (property.value().type() == vtzero::property_value_type::string_value) {
//                    LOGI("key = %s value = %s",
//                         std::string(property.key()).data(),
//                         std::string(property.value().string_value()).data()
//                    );
//                }
            }

            uint64_t featureId = feature.id();
            auto geomType = feature.geometry_type();
            auto props = create_properties_map<layer_map_type>(feature);
            auto styleIndex = style.determineStyle(layerName, props, z);
            if (styleIndex == 0) {
                continue;
            }

            if (geomType == vtzero::GeomType::LINESTRING) {
                auto lineHandler = LineStringHandler();
                vtzero::decode_linestring_geometry(feature.geometry(), lineHandler);
                size_t geomSize = lineHandler.lines.size();
                bool isWideLine = style.getIsWideLine(styleIndex);
                if (isWideLine) {
                    // широкая линия
                    std::vector<MapWideLine> wideLines(geomSize);
                    auto name = boost::get<std::string>(props["name"]);
                    auto wName = Utils::stringToWstring(name);

                    for (size_t geomIndex = 0; geomIndex < geomSize; ++geomIndex) {
                        auto point_array = lineHandler.lines[geomIndex];

                        if (name != "") {
                            // add streets names
                            parseRoadTitleText(wName, point_array, mapSymbols, 2.0);
                        }

                        const size_t pointsSize = point_array.size();
                        std::vector<float> verticesSquarePoints(pointsSize * 4 * 2);

                        // потому что нужна дополнительная точка на каждую центральную точку на дороге
                        size_t pointSizeMidInsert = pointsSize;
                        if (pointsSize > 2) {
                            pointSizeMidInsert += (pointsSize - 2);
                        }

                        size_t wideLinePointsSize = pointSizeMidInsert * 2;
                        wideLinesPointsCount[styleIndex] += wideLinePointsSize;
                        std::vector<float> vertices(wideLinePointsSize * 2);
                        std::vector<float> uv(wideLinePointsSize * 2);
                        std::vector<float> perpendiculars(wideLinePointsSize * 2);
                        std::vector<float> pointsVertices(wideLinePointsSize);
                        std::vector<float> color = CommonUtils::toOpenGlColor(CSSColorParser::parse(Utils::generateRandomColor()));

                        uint verticesIndex = 0;
                        uint uvIndex = 0;
                        uint perpendicularsIndex = 0;
                        uint pointsVerticesIndex = 0;

                        float nx = 0;
                        float ny = 0;
                        for(int pointIndex = 0; pointIndex < pointsSize; ++pointIndex) {
                            auto point = point_array[pointIndex];
                            auto isLastPoint = pointIndex == pointsSize - 1;
                            auto isFirstPoint = pointIndex == 0;

                            // на конечном отрезке мы дублируем точку еще раз с теми же нормалями
                            // то есть вот мы начали вести дорогу, сохраняли нормали один раз,
                            // пришли в следующую точку дороги и опять использовали предыдущие нормали
                            // то есть дорога рисуется прямоугольниками
                            if (isFirstPoint == false && isLastPoint == false) {
                                pointsVertices[pointsVerticesIndex++] = point.x;
                                pointsVertices[pointsVerticesIndex++] = -point.y;

                                vertices[verticesIndex++] = point.x;
                                vertices[verticesIndex++] = -point.y;
                                vertices[verticesIndex++] = point.x;
                                vertices[verticesIndex++] = -point.y;

                                uv[uvIndex++] = 0;
                                uv[uvIndex++] = 0;
                                uv[uvIndex++] = 1;
                                uv[uvIndex++] = 0;

                                perpendiculars[perpendicularsIndex++] = nx;
                                perpendiculars[perpendicularsIndex++] = ny;
                                perpendiculars[perpendicularsIndex++] = -nx;
                                perpendiculars[perpendicularsIndex++] = -ny;
                            }

                            // На каждом стыке дороги рисовать кружочки
                            squarePointsCount[styleIndex] += 1;
                            unsigned int startFrom = pointIndex * 8;
                            verticesSquarePoints[startFrom + 0] = point.x;
                            verticesSquarePoints[startFrom + 1] = -point.y;
                            verticesSquarePoints[startFrom + 2] = point.x;
                            verticesSquarePoints[startFrom + 3] = -point.y;
                            verticesSquarePoints[startFrom + 4] = point.x;
                            verticesSquarePoints[startFrom + 5] = -point.y;
                            verticesSquarePoints[startFrom + 6] = point.x;
                            verticesSquarePoints[startFrom + 7] = -point.y;

                            // рассчитать перпендикуляры относительно следующей точки дороги
                            if (pointIndex + 1 < pointsSize) {
                                vtzero::point nextPoint = point_array[pointIndex + 1];
                                float dx = nextPoint.x - point.x;
                                float dy = point.y - nextPoint.y;
                                nx = -dy;
                                ny = dx;
                                float len = sqrt( pow(nx, 2.0) + pow(ny, 2.0) );
                                nx = nx / len;
                                ny = ny / len;
                            }

//                            if (isLastPoint) {
//                                lastPointsTest.push_back(point.x);
//                                lastPointsTest.push_back(-point.y);
//                            }

//                            if (isFirstPoint) {
//                                firstPointsTest.push_back(point.x);
//                                firstPointsTest.push_back(-point.y);
//                            }

                            pointsVertices[pointsVerticesIndex++] = point.x;
                            pointsVertices[pointsVerticesIndex++] = -point.y;

                            vertices[verticesIndex++] = point.x;
                            vertices[verticesIndex++] = -point.y;
                            vertices[verticesIndex++] = point.x;
                            vertices[verticesIndex++] = -point.y;

                            uv[uvIndex++] = 0;
                            uv[uvIndex++] = 0;
                            uv[uvIndex++] = 1;
                            uv[uvIndex++] = 0;

                            perpendiculars[perpendicularsIndex++] = nx;
                            perpendiculars[perpendicularsIndex++] = ny;
                            perpendiculars[perpendicularsIndex++] = -nx;
                            perpendiculars[perpendicularsIndex++] = -ny;
                        }

                        size_t indicesAmount = (pointSizeMidInsert - 1) * 6;
                        std::vector<unsigned int> indices(indicesAmount);
                        for(int i = 0; i < pointSizeMidInsert - 1; i++) {
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

                        squarePoints[styleIndex].push_front(std::move(verticesSquarePoints));
                    }
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
            } else if (geomType == vtzero::GeomType::POINT) {
                auto pointHandler = PointHandler();
                vtzero::decode_point_geometry(feature.geometry(), pointHandler);
                auto name = style.getName(styleIndex);
                auto wName = Utils::stringToWstring(name);
                auto fontSize = style.getFontSize(styleIndex);
                auto visibleZoom = style.getVisibleZoom(styleIndex);
                if (pointHandler.points.size() == 1 && !name.empty()) {
                    auto point = pointHandler.points[0];
                    float inTilePortion = FLOAT(point.x) / FLOAT(extent);
                    float mapXTilePosition = inTilePortion + getX();
                    int n = pow(2, z);
                    double mapXTilePotion = static_cast<double>(mapXTilePosition) / n;
                    float longitude = mapXTilePotion * (2 * M_PI) - M_PI;

                    float inTilePortionY = FLOAT(point.y) / FLOAT(extent);
                    float mapYTilePosition = inTilePortionY + getY();
                    double mapYTilePortion = 1.0 - static_cast<double>(mapYTilePosition) / n;
                    float latitude = Utils::EPSG3857_to_EPSG4326_latitude(mapYTilePortion * (2 * M_PI) - M_PI);
                    resultMarkerTitles.push_back(MarkerMapTitle(
                            wName,
                            latitude,
                            longitude,
                            fontSize,
                            visibleZoom
                    ));
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

    style.createStylesVec();
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

void MapTile::parseRoadTitleText(
        std::wstring& useStreetName,
        std::vector<vtzero::point>& point_array,
        MapSymbols& mapSymbols,
        float symbolScale
) {
    std::vector<std::tuple<Symbol, float, float, float>> forRender {};
    float textWidth = 0;
    float textHeight = 0;
    float maxTop = 0;
    for (auto charSymbol : useStreetName) {
        Symbol symbol = mapSymbols.getSymbol(charSymbol);
        float w = symbol.width * symbolScale;
        float h = symbol.rows * symbolScale;
        float top = h - symbol.bitmapTop * symbolScale;
        if (top > maxTop) maxTop = top;

        float xPixelsShift = (symbol.advance >> 6) * symbolScale;
        textWidth += xPixelsShift;
        if (textHeight < h + top) textHeight = h + top;
        forRender.push_back({symbol, w, h, xPixelsShift});
    }

    // находим регионы пути где направления соответсвуют определенным
    {
        // ищем регионы первого типа
        std::vector<std::vector<vtzero::point>> regionsType;
        std::vector<vtzero::point> region = { point_array[0] };
        for (int i = 1; i < point_array.size(); i++) {
            auto& firstPoint = point_array[i - 1];
            auto& secondPoint = point_array[i];
            Eigen::Vector2f direction = Eigen::Vector2f(secondPoint.x - firstPoint.x, -secondPoint.y + firstPoint.y);
            bool cond1 = direction.x() > 0 && direction.y() > 0;
            bool cond2 = direction.x() > 0 && direction.y() < 0;
            if (cond1 || cond2) {
                region.push_back(secondPoint);
                continue;
            }

            if (region.size() >= 2) {
                regionsType.push_back(region);
            }

            region.clear();
            region.push_back(secondPoint);
        }
        if (region.size() >= 2) {
            regionsType.push_back(region);
        }


        for(auto& points : regionsType) {
            // Calculate full length of path
            float sumLength = 0;
            for (int i = 1; i < points.size(); i++) {
                auto& firstPoint = points[i - 1];
                auto& secondPoint = points[i];
                float distance = sqrt( pow(firstPoint.x - secondPoint.x, 2.0) + pow(-firstPoint.y + secondPoint.y, 2.0) );
                sumLength += distance;
            }

            if (sumLength - 100 < textWidth) {
                // не поместится поэтому пропускаем
                continue;
            }

            auto randomColor = CommonUtils::toOpenGlColor(CSSColorParser::parse(Utils::generateRandomColor()));
            resultDrawTextAlongPath.push_back(DrawTextAlongPath {
                useStreetName, points, randomColor, 1, forRender,
                textWidth, textHeight, maxTop, sumLength
            });
        }
    }

    {
        // ищем регионы второго типа
        std::vector<std::vector<vtzero::point>> regionsType;
        std::vector<vtzero::point> region = { point_array[0] };
        for (int i = 1; i < point_array.size(); i++) {
            auto& firstPoint = point_array[i - 1];
            auto& secondPoint = point_array[i];
            Eigen::Vector2f direction = Eigen::Vector2f(secondPoint.x - firstPoint.x, -secondPoint.y + firstPoint.y);
            bool cond1 = direction.x() < 0 && direction.y() < 0;
            bool cond2 = direction.x() < 0 && direction.y() > 0;
            if (cond1 || cond2) {
                region.insert(region.begin(), secondPoint);
                continue;
            }

            if (region.size() >= 2) {
                regionsType.push_back(region);
            }

            region.clear();
            region.push_back(secondPoint);
        }
        if (region.size() >= 2) {
            regionsType.push_back(region);
        }

        for(auto& points : regionsType) {
            // Calculate full length of path
            float sumLength = 0;
            for (int i = 1; i < points.size(); i++) {
                auto& firstPoint = points[i - 1];
                auto& secondPoint = points[i];
                float distance = sqrt( pow(firstPoint.x - secondPoint.x, 2.0) + pow(-firstPoint.y + secondPoint.y, 2.0) );
                sumLength += distance;
            }

            if (sumLength - 100 < textWidth) {
                // не поместится поэтому пропускаем
                continue;
            }

            auto randomColor = CommonUtils::toOpenGlColor(CSSColorParser::parse(Utils::generateRandomColor()));
            resultDrawTextAlongPath.push_back(DrawTextAlongPath {
                useStreetName, points, randomColor, 2, forRender,
                textWidth, textHeight, maxTop, sumLength
            });
        }
    }
}



