//
// Created by Artem on 16.09.2024.
//

#include "MapGeometry.h"
#include <vector>

std::vector<float> MapGeometry::generatePlainGeometry(
        float size,
        int segments,
        float radius,
        std::vector<float>& uv,
        std::vector<unsigned int>& indices
) {
    std::vector<float> vertices;
    float xShift = -radius;
    float yShift = -radius;

    float dx = size / segments;
    float dy = size / segments;

    uv.clear();
    indices.clear();

    for (int xi = 0; xi <= segments; xi++) {
        float x = xi * dx + xShift;
        for (int yi = 0; yi <= segments; yi++) {
            float y = yi * dy + yShift;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(radius);

            uv.push_back((float) xi / segments);
            uv.push_back((float) yi / segments);
        }
    }

    for (int xi = 0; xi < segments; xi++) {
        for (int yi = 0; yi < segments; yi++) {
            int i = xi * (segments + 1) + yi;
            indices.push_back(i);
            indices.push_back(i + 1);
            indices.push_back(i + segments + 1);
            indices.push_back(i + 1);
            indices.push_back(i + segments + 1);
            indices.push_back(i + segments + 2);
        }
    }

    return std::move(vertices);
}
