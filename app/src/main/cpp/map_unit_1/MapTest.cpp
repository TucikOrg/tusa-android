//
// Created by Artem on 16.09.2024.
//

#include "MapTest.h"
#include <GLES2/gl2.h>

void MapTest::drawPoints2D(ShadersBucket& shadersBucket, std::vector<float>& vertices, float pointSize, Eigen::Matrix4f& matrix) {
    auto plainShader = shadersBucket.plainShader;
    const GLfloat color[] = { 1, 0, 0, 1};
    glUseProgram(plainShader->program);
    glUniform1f(plainShader->getPointSizeLocation(), pointSize);
    glUniform4fv(plainShader->getColorLocation(), 1, color);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, matrix.data());
    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glDrawArrays(GL_POINTS, 0, vertices.size() / 2);
}

void MapTest::drawPoints3D(
        ShadersBucket &shadersBucket,
        std::vector<float> &vertices,
        float pointSize,
        Eigen::Matrix4f &matrix
) {
    auto plainShader = shadersBucket.plainShader;
    const GLfloat color[] = { 1, 0, 0, 1};
    glUseProgram(plainShader->program);
    glUniform1f(plainShader->getPointSizeLocation(), pointSize);
    glUniform4fv(plainShader->getColorLocation(), 1, color);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, matrix.data());
    glVertexAttribPointer(plainShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glDrawArrays(GL_POINTS, 0, vertices.size() / 3);
}

void MapTest::drawLines3D(
        ShadersBucket &shadersBucket,
        std::vector<float> &vertices,
        std::vector<unsigned int> &indices,
        float lineWidth,
        Eigen::Matrix4f &matrix
) {
    auto plainShader = shadersBucket.plainShader;
    const GLfloat color[] = { 1, 0, 0, 1};
    glUseProgram(plainShader->program);
    glLineWidth(lineWidth);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, matrix.data());
    glVertexAttribPointer(plainShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, indices.data());
}

void MapTest::drawTopText(ShadersBucket &shadersBucket, MapSymbols &mapSymbols, MapCamera& mapCamera, std::string text, float leftD, float topD) {
    CSSColorParser::Color color = CSSColorParser::parse("rgb(255, 0, 0)");
    float aspectRatio = mapCamera.getRatio();
    float left        = -aspectRatio;
    float right       = aspectRatio;
    float bottom      = -1;
    float top         = 1;
    mapSymbols.renderText2D(text, left + leftD, top - topD, 0.001, color, pvUI, shadersBucket);
}

void MapTest::init(MapCamera& mapCamera) {
    float aspectRatio = mapCamera.getRatio();
    float left        = -aspectRatio;
    float right       = aspectRatio;
    float bottom      = -1;
    float top         = 1;
    auto projectionUI = mapCamera.createOrthoProjection(left, right, bottom, top, 0.1, 1);
    auto viewUI = mapCamera.createView();
    pvUI = projectionUI * viewUI;
}
