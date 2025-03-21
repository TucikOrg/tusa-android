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
            auto id = feature.id();
            auto props = create_properties_map<layer_map_type>(feature);
            auto geomType = feature.geometry_type();

            if (layerName == "road") {
                auto name = boost::get<std::string>(props["name"]);
                if (name == "Москворецкая набережная") {
                    int i = 1;
                }
                while (auto property = feature.next_property()) {
                    if (property.key() == "filterrank") {
                        auto type = property.value().type();
                    }
                    if (property.key() == "symbolrank") {
                        auto type = property.value().type();
                    }

//                    if (property.value().type() == vtzero::property_value_type::string_value) {
//                        LOGI("key = %s value = %s",
//                             std::string(property.key()).data(),
//                             std::string(property.value().string_value()).data()
//                        );
//                    }
                }
                int i = 1;
            }

            auto styleIndex = style.determineStyle(layerName, props, z);
            if (styleIndex == 0) {
                continue;
            }

            if (maxStyleIndex < styleIndex) {
                maxStyleIndex = styleIndex;
            }

            if (geomType == vtzero::GeomType::LINESTRING) {
                auto lineHandler = LineStringHandler();
                vtzero::decode_linestring_geometry(feature.geometry(), lineHandler);
                size_t geomSize = lineHandler.lines.size();
                bool isWideLine = style.getIsWideLine(styleIndex);
                if (isWideLine) {
                    // широкая линия
                    std::vector<MapWideLine> wideLines(geomSize);
                    auto nameTest = boost::get<std::string>(props["name"]);
                    auto wNameTest = Utils::stringToWstring(nameTest);
                    auto name = boost::get<std::string>(props["name_ru"]);
                    if (name == "") {
                        name = boost::get<std::string>(props["name_en"]);
                    }
                    if (name == "") {
                        name = nameTest;
                    }
                    auto wName = Utils::stringToWstring(name);


                    std::vector<vtzero::point> sumPoints = {};
                    for (size_t geomIndex = 0; geomIndex < geomSize; ++geomIndex) {
                        auto point_array = lineHandler.lines[geomIndex];
                        //sumPoints.insert(sumPoints.begin(), point_array.begin(), point_array.end());
                        if (name != "") {
                            // Добавить названия на улицы
                            //auto id = boost::get<uint64_t>(props["id"]);
                            parseRoadTitleText(wName, name, point_array, mapSymbols, id);
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
                    float longitude;
                    float latitude;
                    latAndLonFromTilePoint(point, latitude, longitude);

                    // временно символ заменяем Надо потом как-то ё научится рендрить
                    wchar_t old_char = L'ё';
                    wchar_t new_char = L'е';
                    std::replace(wName.begin(), wName.end(), old_char, new_char);

                    std::vector<std::tuple<Symbol, float, float, float>> forRender {};
                    float symbolScale = 1.0;
                    float textureWidth = 0;
                    float textureHeight = 0;
                    float maxTop = 0;
                    std::string::const_iterator iterator;
                    for (auto charSymbol : wName) {
                        Symbol symbol = mapSymbols.getSymbol(charSymbol);
                        float w = symbol.width * symbolScale;
                        float h = symbol.rows * symbolScale;
                        float top = h - symbol.bitmapTop * symbolScale;
                        if (top > maxTop) maxTop = top;

                        float xPixelsShift = (symbol.advance >> 6) * symbolScale;
                        textureWidth += xPixelsShift;
                        if (textureHeight < h + top) textureHeight = h + top;
                        forRender.push_back({symbol, w, h, xPixelsShift});
                    }

                    uint64_t placeLabelKey = CommonUtils::makeKeyFromFloats(latitude, longitude);
                    //auto id = boost::get<uint64_t>(props["id"]);
                    resultMarkerTitles[placeLabelKey] = MarkerMapTitle(
                            wName,
                            latitude,
                            longitude,
                            fontSize,
                            visibleZoom,
                            placeLabelKey,
                            textureWidth,
                            textureHeight,
                            forRender,
                            maxTop,
                            id
                    );
                    // сортируем позже по значению filter
                    resultOrderedMarkerTitles.push_back(&resultMarkerTitles[placeLabelKey]);
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

void MapTile::latAndLonFromTilePoint(vtzero::point point, float& latitude, float& longitude) {
    int useTileX = getX();
    int n = pow(2, getZ());
    int32_t x = point.x;
    int tilesShift = point.x / 4096;
    if (point.x < 0) {
        tilesShift += 1;
        x = 4096 - fmod(abs(point.x), 4096);
        useTileX -= tilesShift;
    } else if (point.x > 4096) {
        x = fmod(point.x, 4096);
        useTileX += tilesShift;
    }
    useTileX = fmod(useTileX, n);
    if (useTileX < 0) {
        useTileX += n;
    }

    float inTilePortion = FLOAT(x) / FLOAT(extent);
    float mapXTilePosition = inTilePortion + useTileX;
    double mapXTilePotion = static_cast<double>(mapXTilePosition) / n;
    longitude = mapXTilePotion * (2 * M_PI) - M_PI;

    float inTilePortionY = FLOAT(point.y) / FLOAT(extent);
    float mapYTilePosition = inTilePortionY + getY();
    double mapYTilePortion = 1.0 - static_cast<double>(mapYTilePosition) / n;
    latitude = Utils::EPSG3857_to_EPSG4326_latitude(mapYTilePortion * (2 * M_PI) - M_PI);
}

void MapTile::parseRoadTitleText(
        std::wstring useStreetName,
        std::string name,
        std::vector<vtzero::point>& pointsRaw,
        MapSymbols& mapSymbols,
        uint64_t roadId
) {
    // временно символ заменяем Надо потом как-то ё научится рендрить
    wchar_t old_char = L'ё';
    wchar_t new_char = L'е';
    std::replace(useStreetName.begin(), useStreetName.end(), old_char, new_char);

//    if (name != "Боровицкая улица") {
//        return;
//    }

    pathIndex++;
    std::vector<std::tuple<Symbol, float, float, float>> forRender {};
    float textureWidth = 0;
    float textureHeight = 0;
    float maxTop = 0;
    for (auto charSymbol : useStreetName) {
        Symbol symbol = mapSymbols.getSymbol(charSymbol);
        float w = symbol.width;
        float h = symbol.rows;
        float top = h - symbol.bitmapTop;
        if (top > maxTop) maxTop = top;

        float xPixelsShift = (symbol.advance >> 6);
        textureWidth += xPixelsShift;
        if (textureHeight < h + top) textureHeight = h + top;
        forRender.push_back({symbol, w, h, xPixelsShift});
    }

//    auto& points = point_array;
//    float sumLength = 0;
//    for (int i = 1; i < points.size(); i++) {
//        auto& firstPoint = points[i - 1];
//        auto& secondPoint = points[i];
//        float distance = sqrt( pow(firstPoint.x - secondPoint.x, 2.0) + pow(-firstPoint.y + secondPoint.y, 2.0) );
//        sumLength += distance;
//    }
//
//    std::vector<float> path = {};
//    for (int i = 0; i < points.size(); i++) {
//        auto& point = points[i];
//        path.push_back(point.x);
//        path.push_back(-point.y);
//    }
//
//    auto randomColor = CommonUtils::toOpenGlColor(CSSColorParser::parse(Utils::generateRandomColor()));
//    resultDrawTextAlongPath.push_back(DrawTextAlongPath {
//            useStreetName, path, randomColor, forRender,
//            textureWidth, textureHeight, maxTop, sumLength, featureId,
//            MapTile::makeKey(getX(), getY(), getZ()), points
//    });
//    return;

    std::vector<vtzero::point> point_array = {};
    for (auto& point : pointsRaw) {
        // если точка за пределами тайла то игнорируем ее для названия улиц
//        if (point.x < 0 || point.x > extent || point.y < 0 || point.y > extent) {
//            continue;
//        }
        point_array.push_back(point);
    }

    if (point_array.size() < 2) {
        return;
    }

    // находим регионы пути где направления соответсвуют определенным
    std::vector<Region> regions;
    for (short t = 0; t <= 1; t++) {
        std::vector<vtzero::point> pointsOfRegion = { point_array[0] };
        for (int i = 1; i < point_array.size(); i++) {
            auto& firstPoint = point_array[i - 1];
            auto& secondPoint = point_array[i];

            Eigen::Vector2f direction = Eigen::Vector2f(secondPoint.x - firstPoint.x, -secondPoint.y + firstPoint.y);
            if (t == 0) {
                bool cond1 = direction.x() > 0 && direction.y() > 0;
                bool cond2 = direction.x() > 0 && direction.y() < 0;
                if (cond1 || cond2) {
                    pointsOfRegion.push_back(secondPoint);
                    continue;
                }
            } else if (t == 1) {
                bool cond1 = direction.x() < 0 && direction.y() < 0;
                bool cond2 = direction.x() < 0 && direction.y() > 0;
                if (cond1 || cond2) {
                    pointsOfRegion.insert(pointsOfRegion.begin(), secondPoint);
                    continue;
                }
            }

            if (pointsOfRegion.size() >= 2) {
                regions.push_back({t, pointsOfRegion, name, pathIndex});
            }

            pointsOfRegion.clear();
            pointsOfRegion.push_back(secondPoint);
        }
        if (pointsOfRegion.size() >= 2) {
            regions.push_back({t, pointsOfRegion, name, pathIndex});
        }
    }

    // regions это участки одного пути Одного feature разделенные так чтобы текст правильно ложился
    for(short regionId = 0; regionId < regions.size(); regionId++) {
        auto& region = regions[regionId];
        auto& regionPoints = region.points;

        // Calculate full length of path
        float sumLength = 0;
        for (int i = 1; i < regionPoints.size(); i++) {
            auto& firstPoint = regionPoints[i - 1];
            auto& secondPoint = regionPoints[i];
            float distance = sqrt( pow(firstPoint.x - secondPoint.x, 2.0) + pow(-firstPoint.y + secondPoint.y, 2.0) );
            sumLength += distance;
        }

        std::vector<float> path = {};
        for (int i = 0; i < regionPoints.size(); i++) {
            auto& point = regionPoints[i];
            path.push_back(point.x);
            path.push_back(-point.y);
        }

        auto randomColor = CommonUtils::toOpenGlColor(CSSColorParser::parse(Utils::generateRandomColor()));
        resultDrawTextAlongPath.push_back(DrawTextAlongPath {
                useStreetName, path, randomColor, forRender,
                textureWidth, textureHeight, maxTop, sumLength,
                MapTile::makeKey(getX(), getY(), getZ()), regionPoints, region.type,
                roadId, regionId
        });
    }
}

void MapTile::destroy() {
    for (auto& linesPair : resultLinesAggregatedByStyles) {
        auto& mapSimpleLine = linesPair.second;
        glDeleteFramebuffers(1, &mapSimpleLine.vbo);
        glDeleteFramebuffers(1, &mapSimpleLine.ibo);
    }

    for (auto& polygonsPair : resultPolygonsAggregatedByStyles) {
        auto& mapPolygon = polygonsPair.second;
        glDeleteFramebuffers(1, &mapPolygon.vbo);
        glDeleteFramebuffers(1, &mapPolygon.ibo);
    }

    for (auto& pointsPair : resultPointsAggregatedByStyles) {
        auto& mapSimplePoint = pointsPair.second;
        glDeleteFramebuffers(1, &mapSimplePoint.vbo);
    }

    for (auto& squarePointsPair : resultSquarePointsAggregatedByStyles) {
        auto& mapSquarePoints = squarePointsPair.second;
        glDeleteFramebuffers(1, &mapSquarePoints.vbo);
        glDeleteFramebuffers(1, &mapSquarePoints.ibo);
        glDeleteFramebuffers(1, &mapSquarePoints.vboUVs);
        glDeleteFramebuffers(1, &mapSquarePoints.vboShifts);
    }

    for (auto& wideLinesPair : resultWideLineAggregatedByStyles) {
        auto& mapWideLine = wideLinesPair.second;
        glDeleteFramebuffers(1, &mapWideLine.vbo);
        glDeleteFramebuffers(1, &mapWideLine.vboUv);
        glDeleteFramebuffers(1, &mapWideLine.ibo);
        glDeleteFramebuffers(1, &mapWideLine.vboPerpendiculars);
    }

    for (auto& drawTextAlongPath : resultDrawTextAlongPath) {
        glDeleteFramebuffers(1, &drawTextAlongPath.ibo);
    }
}



