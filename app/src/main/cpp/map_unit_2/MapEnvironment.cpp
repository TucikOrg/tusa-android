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

void MapEnvironment::createStars(float planeSize) {
    std::random_device rd;
    std::mt19937 gen(rd());
    float cubeOuterBorder = planeSize * 1.5;
    float cubeInnerBorder = planeSize;
    std::uniform_int_distribution<> coordinateRange(0, cubeOuterBorder);
    std::uniform_int_distribution<> randomDirNumber(0, 1);
    std::uniform_int_distribution<> randomStarSize(1, 12);
    std::vector<float> starsSize = {};
    std::vector<float> starsVertices = {};

    for (int i = 0; i < 2000; i++) {
        float dirX = randomDirNumber(gen) == 0 ? -1.0f : 1.0f;
        float dirY = randomDirNumber(gen) == 0 ? -1.0f : 1.0f;
        float dirZ = randomDirNumber(gen) == 0 ? -1.0f : 1.0f;
        float vertexX = coordinateRange(gen) * dirX;
        float vertexY = coordinateRange(gen) * dirY;
        float vertexZ = coordinateRange(gen) * dirZ;
        float dist = sqrt(vertexX * vertexX + vertexY * vertexY + vertexZ * vertexZ);
        if (dist < cubeInnerBorder)
            continue;
        starsVertices.push_back(vertexX);
        starsVertices.push_back(vertexY);
        starsVertices.push_back(vertexZ);
        starsSize.push_back(randomStarSize(gen));
    }
    starsAmount = starsVertices.size() / 3;

    glGenBuffers(1, &starsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, starsVbo);
    glBufferData(GL_ARRAY_BUFFER, starsVertices.size() * sizeof(float), starsVertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &starsSizeVbo);
    glBindBuffer(GL_ARRAY_BUFFER, starsSizeVbo);
    glBufferData(GL_ARRAY_BUFFER, starsSize.size() * sizeof(float), starsSize.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MapEnvironment::init(float planeSize) {
    createTopCap(planeSize);
    createBottomCap(planeSize);
    createStars(planeSize);
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

void MapEnvironment::drawStars(
        MapNumbers& mapNumbers,
        ShadersBucket &shadersBucket
) {
    Eigen::Matrix4f rotateLatitude = EigenGL::createRotationMatrixAxis(FLOAT(mapNumbers.camLatitude), Eigen::Vector3f {1.0f, 0.0f, 0.0f});
    Eigen::Matrix4f rotateLongitude = EigenGL::createRotationMatrixAxis(FLOAT(mapNumbers.camLongitude), Eigen::Vector3f {0.0f, -1.0f, 0.0f});
    Eigen::Matrix4f pvm = mapNumbers.pvFloat * rotateLatitude * rotateLongitude;
    auto starsColor = MapColors::getStarsColor();
    auto starsColorGl = CommonUtils::toOpenGlColor(starsColor);
    auto shader = shadersBucket.starsShader.get();
    glUseProgram(shader->program);
    glUniformMatrix4fv(shader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
    glUniform4fv(shader->getColorLocation(), 1, starsColorGl.data());

    glBindBuffer(GL_ARRAY_BUFFER, starsVbo);
    glVertexAttribPointer(shader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shader->getPosLocation());

    glBindBuffer(GL_ARRAY_BUFFER, starsSizeVbo);
    glVertexAttribPointer(shader->getPointSizeLocation(), 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shader->getPointSizeLocation());
    glDrawArrays(GL_POINTS, 0, starsAmount);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MapEnvironment::draw(
        MapNumbers& mn,
        ShadersBucket &shadersBucket
) {
    float planeSize = mn.planeSize;
    if (mn.zoom < drawSpaceZoomBorder) {
        drawStars(mn, shadersBucket);
        drawGlowing(mn, shadersBucket);
    }
    glEnable(GL_DEPTH_TEST);
    drawTopCap(mn.pvFloat, mn.sphereModelMatrixFloat, mn.zoom, planeSize, shadersBucket);
    drawBottomCap(mn.pvFloat, mn.sphereModelMatrixFloat, mn.zoom, planeSize, shadersBucket);
    glDisable(GL_DEPTH_TEST);
}

void MapEnvironment::selectClearColor(float zoom) {
    if (zoom < drawSpaceZoomBorder) {
        auto spaceColor = CommonUtils::toOpenGlColor(MapColors::getSpaceColor());
        glClearColor(spaceColor[0], spaceColor[1], spaceColor[2], spaceColor[3]);
    } else {
        auto backgroundColor = CommonUtils::toOpenGlColor(MapColors::getBackground());
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
    }
}

void MapEnvironment::drawGlowing(
        MapNumbers &mapNumbers,
        ShadersBucket &shadersBucket
) {
    float radius = mapNumbers.radius;
    float glowRadius = radius * 2.0f;
    Eigen::Matrix4f pv = mapNumbers.pvFloat;
    std::vector<float> verticesPlanetGlow = {
            -glowRadius, -glowRadius, -radius,
            glowRadius, -glowRadius, -radius,
            glowRadius, glowRadius, -radius,
            -glowRadius, glowRadius, -radius
    };
    std::vector<float> uv = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
    };
    float size = 20000.0f;

    auto glowShader = shadersBucket.planetGlowShader.get();
    glUseProgram(glowShader->program);
    glUniformMatrix4fv(glowShader->getMatrixLocation(), 1, GL_FALSE, mapNumbers.pvFloat.data());
    glVertexAttribPointer(glowShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, verticesPlanetGlow.data());
    glEnableVertexAttribArray(glowShader->getPosLocation());
    glVertexAttribPointer(glowShader->getUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, uv.data());
    glEnableVertexAttribArray(glowShader->getUVLocation());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    float darkRadius = radius * 10.0f;
    std::vector<float> verticesSpaceDark = {
            -darkRadius, -darkRadius, -radius,
            darkRadius,-darkRadius, -radius,
            darkRadius, darkRadius, -radius,
            -darkRadius, darkRadius, -radius
    };

    auto spaceDarkShader = shadersBucket.spaceDarkShader.get();
    glUseProgram(spaceDarkShader->program);
    glUniformMatrix4fv(spaceDarkShader->getMatrixLocation(), 1, GL_FALSE, mapNumbers.pvFloat.data());
    glVertexAttribPointer(spaceDarkShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, verticesSpaceDark.data());
    glEnableVertexAttribArray(spaceDarkShader->getPosLocation());
    glVertexAttribPointer(spaceDarkShader->getUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, uv.data());
    glEnableVertexAttribArray(spaceDarkShader->getUVLocation());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void MapEnvironment::destroy() {
    glDeleteBuffers(1, &topCapVbo);
    glDeleteBuffers(1, &topCapIbo);
    glDeleteBuffers(1, &bottomCapVbo);
    glDeleteBuffers(1, &bottomCapIbo);
    glDeleteBuffers(1, &starsVbo);
    glDeleteBuffers(1, &starsSizeVbo);
}
