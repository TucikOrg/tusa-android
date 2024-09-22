//
// Created by Artem on 16.09.2024.
//

#include "MapTest.h"
#include <GLES2/gl2.h>

void MapTest::drawTilesTextureTest(
        ShadersBucket& shadersBucket,
        MapCamera& mapCamera,
        GLuint renderMapTexture,
        int xSize,
        int ySize
) {
    auto textureShader = shadersBucket.textureShader;
    auto view = mapCamera.createView();
    auto ratio = mapCamera.getRatio();
    auto projection = mapCamera.createOrthoProjection(-ratio, ratio, -1, 1, 0.0, 1.0);
    Eigen::Matrix4f pv = projection * view;

    float sizeOfUnit = 0.2;
    float width = sizeOfUnit * xSize;
    float height = sizeOfUnit * ySize;
    std::vector<float> vertices = {
            -ratio, -1,
            -ratio + width, -1,
            -ratio + width, -1 + height,
            -ratio, -1 + height
    };
    std::vector<float> uv = {
            0, 0,
            1, 0,
            1, 1,
            0, 1
    };

    glUseProgram(textureShader->program);
    glUniformMatrix4fv(textureShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
    //glUniform4fv(textureShader->getColorLocation(), 1, new GLfloat[4]{0, 0, 0, 1});
    glBindTexture(GL_TEXTURE_2D, renderMapTexture);
    glUniform1i(textureShader->getTileTextureLocation0(), 0);
    glVertexAttribPointer(textureShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, uv.data());
    glEnableVertexAttribArray(textureShader->getTextureCord());
    glVertexAttribPointer(textureShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glEnableVertexAttribArray(textureShader->getPosLocation());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

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

void MapTest::drawFPS(ShadersBucket &shadersBucket, MapSymbols &mapSymbols, MapCamera& mapCamera, float fps) {
    CSSColorParser::Color color = CSSColorParser::parse("rgb(255, 0, 0)");
    float aspectRatio = mapCamera.getRatio();
    float left        = -aspectRatio;
    float right       = aspectRatio;
    float bottom      = -1;
    float top         = 1;
    mapSymbols.renderText2D("FPS " + std::to_string((int)fps), left + 0.1, top - 0.05, 0.001, color, pvUI, shadersBucket);
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
