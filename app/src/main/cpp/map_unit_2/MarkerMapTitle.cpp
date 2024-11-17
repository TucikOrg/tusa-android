//
// Created by Artem on 11.11.2024.
//

#include "MarkerMapTitle.h"

void MarkerMapTitle::draw(ShadersBucket &shadersBucket,
                          Eigen::Matrix4f &pv,
                          MapNumbers &mapNumbers,
                          FromLatLonToSpherePos &fromLatLonToSpherePos,
                          MapSymbols& mapSymbols,
                          MapCamera& mapCamera
) {
    glDisable(GL_DEPTH_TEST);
    auto camLatitude = mapNumbers.camLatitude;
    auto camLongitudeNormalized = mapNumbers.camLongitudeNormalized;
    double noZTestDelta = M_PI / 2;
    bool visibleZoomSkip = visibleZoom.find(mapNumbers.tileZ) == visibleZoom.end();
    bool enableDepthTest = abs(camLatitude - latitude) > noZTestDelta || abs(camLongitudeNormalized - longitude) > noZTestDelta;
    if (enableDepthTest || visibleZoomSkip) {
        return;
    }

    Eigen::Vector3d position = fromLatLonToSpherePos.getPoint(mapNumbers, latitude, longitude);
    float markerX = position[0];
    float markerY = position[1];
    float markerZ = position[2];
    float scale = mapNumbers.scale * mapNumbers.distortionDistanceToMapPortion;

    CSSColorParser::Color color = CSSColorParser::parse("rgb(0, 0, 0)");
    auto textTexture = mapSymbols.getTextTexture(wname);
    if (textTexture.isEmpty()) {
        textTexture = mapSymbols.renderTextTexture(wname, color, shadersBucket, mapCamera, 2.0f);
    }

    float scaleSizeOfTexture = 0.005 * scale;
    float width = textTexture.width * scaleSizeOfTexture;
    float height = textTexture.height * scaleSizeOfTexture;
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    float vertices[] = {
            markerX - halfWidth, markerY - halfHeight, markerZ,
            markerX + halfWidth, markerY - halfHeight, markerZ,
            markerX + halfWidth, markerY + halfHeight, markerZ,
            markerX - halfWidth, markerY + halfHeight, markerZ
    };
    std::vector<float> uv = {
            0, 0,
            1, 0,
            1, 1,
            0, 1
    };
    auto textureShader = shadersBucket.textureShader;
    glUseProgram(textureShader->program);
    glUniformMatrix4fv(textureShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
    glVertexAttribPointer(textureShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(textureShader->getPosLocation());
    glVertexAttribPointer(textureShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, uv.data());
    glEnableVertexAttribArray(textureShader->getTextureCord());
    glBindTexture(GL_TEXTURE_2D, textTexture.textureId);
    glUniform1f(textureShader->getTextureLocation(), 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
