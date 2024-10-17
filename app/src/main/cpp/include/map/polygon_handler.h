
#include <vector>
#include <geometry.hpp>
#include <array>

#ifndef TUSA_POLYGON_HANDLER
#define TUSA_POLYGON_HANDLER

class PolygonHandler {
public:
    std::vector<std::array<float, 2>> points;
    std::vector<std::vector<std::array<float, 2>>> polygon;
    std::vector<std::vector<std::vector<std::array<float, 2>>>> polygons;
    std::vector<std::array<float, 2>> featurePoints = {};
    int pointsCount = 0;

    void ring_begin(uint32_t count) {
        points.clear();
        points.reserve(count);
    }

    void ring_point(vtzero::point point) {
        points.push_back({(float)point.x, (float)point.y});
        featurePoints.push_back({(float)point.x, (float)point.y});
        pointsCount++;
    }

    void ring_end(vtzero::ring_type type) {
        if(type == vtzero::ring_type::outer && polygon.size() > 0) {
            polygons.push_back(polygon);
            polygon.clear();
        }

        polygon.push_back(points);
    }
};

#endif