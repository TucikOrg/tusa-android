//
// Created by Artem on 15.10.2024.
//

#include "MapEnvironment.h"

#include "util/android_log.h"
#include "util/eigen_gl.h"


void MapEnvironment::createTopCap(float planeSize) {
    float radius = planeSize / (2.0 * M_PI);
    int segments = 40;
    int pointsCount = (segments + 1);
    int lastPoint = pointsCount - 1;
    std::vector<float> vertices(pointsCount * 3);
    vertices[0] = 0.0f;
    vertices[1] = radius;
    vertices[2] = 0.0f;
    float maxLatDeg = RAD2DEG(maxLat);
    int vertIndex = 2;
    int delta = 360 / segments;
    for (int lon = -180; lon <= 180; lon += delta) {
        float lonRad = DEG2RAD(lon);
        float x = radius * cos(maxLat) * sin(lonRad);
        float y = radius * sin(maxLat);
        float z = radius * cos(maxLat) * cos(lonRad);
        vertices[++vertIndex] = x;
        vertices[++vertIndex] = y;
        vertices[++vertIndex] = z;
    }
    translateSphere = EigenGL::createTranslationMatrix(0.0f, 0.0f, -radius);

    glGenBuffers(1, &topCapVbo);
    glBindBuffer(GL_ARRAY_BUFFER, topCapVbo);
    auto data = vertices.data();
    auto size = vertices.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    int indicesAmount = segments * 3;
    topCapIboSize = indicesAmount;
    std::vector<unsigned int> indices(indicesAmount);
    unsigned int indexIndic = 0;
    for (int i = 0; i < lastPoint - 1; i++) {
        indices[indexIndic++] = 0;
        indices[indexIndic++] = i + 1;
        indices[indexIndic++] = i + 2;
    }
    indices[indexIndic++] = 0;
    indices[indexIndic++] = lastPoint;
    indices[indexIndic++] = 1;

    glGenBuffers(1, &topCapIbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topCapIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MapEnvironment::createBottomCap(float planeSize) {
    float radius = planeSize / (2.0 * M_PI);
    int segments = 40;
    int pointsCount = (segments + 1);
    int lastPoint = pointsCount - 1;
    std::vector<float> vertices(pointsCount * 3);
    vertices[0] = 0.0f;
    vertices[1] = -radius;
    vertices[2] = 0.0f;
    float maxLatDeg = RAD2DEG(-maxLat);
    int vertIndex = 2;
    int delta = 360 / segments;
    for (int lon = -180; lon <= 180; lon += delta) {
        float lonRad = DEG2RAD(lon);
        float x = radius * cos(-maxLat) * sin(lonRad);
        float y = radius * sin(-maxLat);
        float z = radius * cos(-maxLat) * cos(lonRad);
        vertices[++vertIndex] = x;
        vertices[++vertIndex] = y;
        vertices[++vertIndex] = z;
    }

    glGenBuffers(1, &bottomCapVbo);
    glBindBuffer(GL_ARRAY_BUFFER, bottomCapVbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    int indicesAmount = segments * 3;
    std::vector<unsigned int> indices(indicesAmount);
    unsigned int indexIndic = 0;
    for (int i = 0; i < lastPoint - 1; i++) {
        indices[indexIndic++] = 0;
        indices[indexIndic++] = i + 1;
        indices[indexIndic++] = i + 2;
    }
    indices[indexIndic++] = 0;
    indices[indexIndic++] = lastPoint;
    indices[indexIndic++] = 1;

    glGenBuffers(1, &bottomCapIbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottomCapIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    bottomCapIboSize = indicesAmount;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MapEnvironment::init(float planeSize) {
    createTopCap(planeSize);
    createBottomCap(planeSize);
}

void MapEnvironment::drawTopCap(Eigen::Matrix4f pv, Eigen::Matrix4f sphereModelMatrix,
                                float zoom, float planeSize, ShadersBucket& shadersBucket) {
    Eigen::Matrix4f pvm = pv * translateSphere * sphereModelMatrix;
    auto waterColorCSS = MapColors::getWaterColor();
    auto waterColor = CommonUtils::toOpenGlColor(waterColorCSS);
    auto alphaFrom = 3.0f;
    auto alphaTo = 4.0f;
    float alpha = 1.0;
    if (zoom > alphaFrom && zoom < alphaTo) {
        alpha = (alphaTo - zoom) / (alphaTo - alphaFrom);
    }
    if (zoom >= alphaTo) {
        alpha = 0.0;
    }
    waterColor[3] = alpha;
    auto plainShader = shadersBucket.plainShader.get();
    glBindBuffer(GL_ARRAY_BUFFER, topCapVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topCapIbo);
    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glVertexAttribPointer(plainShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glUniform4fv(plainShader->getColorLocation(), 1, waterColor.data());
    glUniform1f(plainShader->getPointSizeLocation(), 20.0f);
    glDrawElements(GL_TRIANGLES, topCapIboSize, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MapEnvironment::drawBottomCap(
        Eigen::Matrix4f pv,
        Eigen::Matrix4f sphereModelMatrix,
        float zoom,
        float planeSize,
        ShadersBucket& shadersBucket
) {
    Eigen::Matrix4f pvm = pv * translateSphere * sphereModelMatrix;
    auto poleColor = MapColors::getPoleColor();
    auto alphaFrom = 3.0f;
    auto alphaTo = 4.0f;
    float alpha = 1.0;
    if (zoom > alphaFrom && zoom < alphaTo) {
        alpha = (alphaTo - zoom) / (alphaTo - alphaFrom);
    }
    if (zoom >= alphaTo) {
        alpha = 0.0;
    }
    poleColor[3] = alpha;
    auto plainShader = shadersBucket.plainShader.get();

    glBindBuffer(GL_ARRAY_BUFFER, bottomCapVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottomCapIbo);
    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
    glVertexAttribPointer(plainShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glUniform4fv(plainShader->getColorLocation(), 1, poleColor.data());
    glDrawElements(GL_TRIANGLES, bottomCapIboSize, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
