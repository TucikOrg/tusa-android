//
// Created by Artem on 17.11.2024.
//

#ifndef TUSA_ANDROID_TRANSFERTILESTOGPU_H
#define TUSA_ANDROID_TRANSFERTILESTOGPU_H

#include "MapTile.h"

class TransferTilesToGPU {
public:
    void transfer(std::unordered_map<uint64_t, MapTile*> tilesMap) {
        for (auto& tileMap : tilesMap) {
            auto tile = tileMap.second;
            if (tile->uploadedToGPU) continue;
            auto& resultPolygons = tile->resultPolygonsAggregatedByStyles;
            for (auto& polygonStylePair : resultPolygons) {
                auto& polygon = polygonStylePair.second;
                auto& vertices = polygon.vertices;
                GLuint vbo;
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                auto data = vertices.data();
                auto size = vertices.size() * sizeof(float);
                glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

                auto& indices = polygon.indices;
                GLuint ibo;
                glGenBuffers(1, &ibo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

                polygon.vbo = vbo;
                polygon.ibo = ibo;
                polygon.iboSize = polygon.indices.size();
                polygon.vertices.clear();
                polygon.indices.clear();
            }

            auto& resultLines = tile->resultLinesAggregatedByStyles;
            for (auto& lineStylePair : resultLines) {
                auto& line = lineStylePair.second;
                auto& vertices = line.vertices;
                GLuint vbo;
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                auto data = vertices.data();
                auto size = vertices.size() * sizeof(float);
                glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

                auto& indices = line.indices;
                GLuint ibo;
                glGenBuffers(1, &ibo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

                line.vbo = vbo;
                line.ibo = ibo;
                line.iboSize = line.indices.size();
                line.vertices.clear();
                line.indices.clear();
            }

            auto& resultWideLines = tile->resultWideLineAggregatedByStyles;
            for (auto& wideLineStylePair : resultWideLines) {
                auto& wideLine = wideLineStylePair.second;
                auto& vertices = wideLine.vertices;
                GLuint vbo;
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                auto data = vertices.data();
                auto size = vertices.size() * sizeof(float);
                glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

                auto& perpendiculars = wideLine.perpendiculars;
                GLuint vboPerpendiculars;
                glGenBuffers(1, &vboPerpendiculars);
                glBindBuffer(GL_ARRAY_BUFFER, vboPerpendiculars);
                auto dataPerpendiculars = perpendiculars.data();
                auto sizePerpendiculars = perpendiculars.size() * sizeof(float);
                glBufferData(GL_ARRAY_BUFFER, sizePerpendiculars, dataPerpendiculars, GL_STATIC_DRAW);

                auto& uv = wideLine.uv;
                GLuint vboUv;
                glGenBuffers(1, &vboUv);
                glBindBuffer(GL_ARRAY_BUFFER, vboUv);
                auto dataUv = uv.data();
                auto sizeUv = uv.size() * sizeof(float);
                glBufferData(GL_ARRAY_BUFFER, sizeUv, dataUv, GL_STATIC_DRAW);

                auto& indices = wideLine.indices;
                GLuint ibo;
                glGenBuffers(1, &ibo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

                wideLine.vboUv = vboUv;
                wideLine.vbo = vbo;
                wideLine.vboPerpendiculars = vboPerpendiculars;
                wideLine.ibo = ibo;
                wideLine.iboSize = wideLine.indices.size();
                wideLine.verticesSize = wideLine.vertices.size();

                wideLine.uv.clear();
                wideLine.perpendiculars.clear();
                wideLine.vertices.clear();
                wideLine.indices.clear();
            }

            auto& resultSquarePoints = tile->resultSquarePointsAggregatedByStyles;
            for (auto& squarePointStylePair : resultSquarePoints) {
                auto& squarePoint = squarePointStylePair.second;

                auto& vertices = squarePoint.vertices;
                GLuint vbo;
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                auto data = vertices.data();
                auto size = vertices.size() * sizeof(float);
                glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

                auto& shifts = squarePoint.shifts;
                GLuint vboShifts;
                glGenBuffers(1, &vboShifts);
                glBindBuffer(GL_ARRAY_BUFFER, vboShifts);
                auto dataShifts = shifts.data();
                auto sizeShifts = shifts.size() * sizeof(float);
                glBufferData(GL_ARRAY_BUFFER, sizeShifts, dataShifts, GL_STATIC_DRAW);

                auto& uvs = squarePoint.uvs;
                GLuint vboUvs;
                glGenBuffers(1, &vboUvs);
                glBindBuffer(GL_ARRAY_BUFFER, vboUvs);
                auto dataUvs = uvs.data();
                auto sizeUvs = uvs.size() * sizeof(float);
                glBufferData(GL_ARRAY_BUFFER, sizeUvs, dataUvs, GL_STATIC_DRAW);

                auto& indices = squarePoint.indices;
                GLuint ibo;
                glGenBuffers(1, &ibo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

                squarePoint.vbo = vbo;
                squarePoint.ibo = ibo;
                squarePoint.vboShifts = vboShifts;
                squarePoint.vboUVs = vboUvs;
                squarePoint.iboSize = squarePoint.indices.size();

                squarePoint.shifts.clear();
                squarePoint.vertices.clear();
                squarePoint.indices.clear();
                squarePoint.uvs.clear();
            }

            tile->uploadedToGPU = true;
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
};


#endif //TUSA_ANDROID_TRANSFERTILESTOGPU_H
