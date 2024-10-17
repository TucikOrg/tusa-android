//
// Created by Artem on 16.09.2024.
//

#ifndef TUSA_ANDROID_MAPTEST_H
#define TUSA_ANDROID_MAPTEST_H


#include "shader/shaders_bucket.h"
#include "MapCamera.h"
#include "MapTile.h"
#include "MapTileGetter.h"
#include "MapTileRender.h"
#include "util/android_log.h"
#include "MapStyle.h"
#include "MapSymbols.h"
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/StdVector>

class MapTest {
public:
    void drawTilesTextureTest(
            ShadersBucket& shadersBucket,
            MapCamera& mapCamera,
            GLuint mapTextureForPlanet,
            int visibleWindowX,
            int visibleWindowY
    ) {
        auto textureShader = shadersBucket.textureShader;
        auto plainShader = shadersBucket.plainShader;
        float ratio = mapCamera.getRatio();
        float testTextureWindowSize = 0.2;
        float width = testTextureWindowSize * visibleWindowX;
        float height = testTextureWindowSize * visibleWindowY;
        std::vector<float> testVertices = {
                -ratio, -1.0f,
                -ratio + width, -1.0f,
                -ratio + width, -1.0f + height,
                -ratio, -1.0f + height
        };
        std::vector<float> uvTest = {
                0, 0,
                1, 0,
                1, 1,
                0, 1
        };
        Eigen::Matrix4f projectionTest = mapCamera.createOrthoProjection(-ratio, ratio, -1, 1, 0.1, 1);
        Eigen::Matrix4f viewTest = mapCamera.createView(0, 0, 1, 0, 0, 0);
        Eigen::Matrix4f pvTest = projectionTest * viewTest;
        glUseProgram(textureShader->program);
        glUniformMatrix4fv(textureShader->getMatrixLocation(), 1, GL_FALSE, pvTest.data());
        glVertexAttribPointer(textureShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, testVertices.data());
        glEnableVertexAttribArray(textureShader->getPosLocation());
        glUniform4f(plainShader->getColorLocation(), 1.0, 0.0, 0.0, 1.0);
        glVertexAttribPointer(textureShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, uvTest.data());
        glEnableVertexAttribArray(textureShader->getTextureCord());
        glBindTexture(GL_TEXTURE_2D, mapTextureForPlanet);
        glUniform1f(textureShader->getTileTextureLocation0(), 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    void drawTopText(ShadersBucket &shadersBucket, MapSymbols &mapSymbols, MapCamera& mapCamera, std::string text, float leftD, float topD);
    void drawPoints2D(ShadersBucket& shadersBucket, std::vector<float>& vertices, float pointSize, Eigen::Matrix4f& matrix);
    void drawPoints3D(ShadersBucket& shadersBucket, std::vector<float>& vertices, float pointSize, Eigen::Matrix4f& matrix);
    void drawLines3D(ShadersBucket& shadersBucket, std::vector<float>& vertices, std::vector<unsigned int> &indices, float lineWidth, Eigen::Matrix4f& matrix);
    void drawCenterPoint(ShadersBucket& shadersBucket, Eigen::Matrix4f& pvFloat) {
        auto plainShader = shadersBucket.plainShader;
        glUseProgram(plainShader->program);
        glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvFloat.data());
        float centerPoint[] = {0, 0};
        glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, centerPoint);
        glEnableVertexAttribArray(plainShader->getPosLocation());
        glUniform4f(plainShader->getColorLocation(), 1.0, 0.0, 0.0, 1.0);
        glUniform1f(plainShader->getPointSizeLocation(), 10.0f);
        glDrawArrays(GL_POINTS, 0, 1);
    }
    void drawTextureTest(ShadersBucket& shadersBucket, MapCamera& mapCamera, GLuint mapTexture, int xTilesAmount, int yTilesAmount) {
        auto plainShader = shadersBucket.plainShader;
        // draw texture test
        auto textureShader = shadersBucket.textureShader;
        float ratio = mapCamera.getRatio();
        float testTextureWindowSize = 0.1;
        float width = testTextureWindowSize * xTilesAmount;
        float height = testTextureWindowSize * yTilesAmount;
        std::vector<float> testVertices = {
                -ratio, -1.0f,
                -ratio + width, -1.0f,
                -ratio + width, -1.0f + height,
                -ratio, -1.0f + height
        };
        std::vector<float> uvTest = {
                0, 0,
                1, 0,
                1, 1,
                0, 1
        };
        Eigen::Matrix4f projectionTest = mapCamera.createOrthoProjection(-ratio, ratio, -1, 1, 0.1, 1);
        Eigen::Matrix4f viewTest = mapCamera.createView(0, 0, 1, 0, 0, 0);
        Eigen::Matrix4f pvTest = projectionTest * viewTest;
        glUseProgram(textureShader->program);
        glUniformMatrix4fv(textureShader->getMatrixLocation(), 1, GL_FALSE, pvTest.data());
        glVertexAttribPointer(textureShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, testVertices.data());
        glEnableVertexAttribArray(textureShader->getPosLocation());
        glUniform4f(plainShader->getColorLocation(), 1.0, 0.0, 0.0, 1.0);
        glVertexAttribPointer(textureShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, uvTest.data());
        glEnableVertexAttribArray(textureShader->getTextureCord());
        glBindTexture(GL_TEXTURE_2D, mapTexture);
        glUniform1f(textureShader->getTileTextureLocation0(), 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }


    void init(MapCamera& mapCamera);
private:
    Eigen::Matrix4f pvUI;
};


#endif //TUSA_ANDROID_MAPTEST_H
