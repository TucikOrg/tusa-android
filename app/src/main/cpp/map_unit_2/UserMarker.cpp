//
// Created by Artem on 17.10.2024.
//

#include "UserMarker.h"

void UserMarker::draw(
        ShadersBucket &shadersBucket,
        Eigen::Matrix4f &pv,
        MapNumbers& mapNumbers,
        FromLatLonToSpherePos& fromLatLonToSpherePos
) {
    auto camLatitude = mapNumbers.camLatitude;
    auto camLongitudeNormalized = mapNumbers.camLongitudeNormalized;

    double noZTestDelta = M_PI / 4;
    bool enableDepthTest = abs(camLatitude - latitude) > noZTestDelta || abs(camLongitudeNormalized - longitude) > noZTestDelta;
    if (enableDepthTest) {
        glEnable(GL_DEPTH_TEST);
    }

    Eigen::Vector3d position = fromLatLonToSpherePos.getPoint(mapNumbers, latitude, longitude);
    std::vector<float> uv = {
            0, 1,
            1, 1,
            1, 0,
            0, 0,
    };
    float markerX = position[0];
    float markerY = position[1];
    float markerZ = position[2];

    float z = 0.0f;
    float scale = mapNumbers.scale * mapNumbers.distortionDistanceToMapPortion;
    float scaledSize = size * scale;
    float halfSize = scaledSize / 2;
    std::vector<float> vertices = {
            markerX - halfSize, markerY + 0, markerZ + z,
            markerX + halfSize, markerY + 0, markerZ + z,
            markerX + halfSize, markerY + scaledSize, markerZ + z,
            markerX - halfSize, markerY + scaledSize, markerZ + z,
    };
    auto shader = shadersBucket.userMarkerShader.get();
    glUseProgram(shader->program);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glUniform1i(shader->getTextureLocation(), 0);
    glUniformMatrix4fv(shader->getMatrixLocation(), 1, GL_FALSE, pv.data());
    glVertexAttribPointer(shader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glEnableVertexAttribArray(shader->getPosLocation());
    glVertexAttribPointer(shader->getUVLocation(), 2, GL_FLOAT, GL_FALSE, 0, uv.data());
    glEnableVertexAttribArray(shader->getUVLocation());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisable(GL_DEPTH_TEST);

    //drawDebugData(shadersBucket, pv, mapNumbers, markerX, markerY, markerZ, radius, axisLatitude, axisLongitude);
}

UserMarker::UserMarker(GLuint textureId, float latitude, float longitude)
    : textureId(textureId), latitude(latitude), longitude(longitude) {
}

void UserMarker::drawDebugData(
        ShadersBucket &shadersBucket,
        Eigen::Matrix4f pv,
        MapNumbers &mapNumbers,
        float markerX,
        float markerY,
        float markerZ,
        float radius,
        Eigen::Vector3d axisLatitude,
        Eigen::Vector3d axisLongitude
) {

    std::vector<float> pointVertices = {
            markerX, markerY, markerZ,
    };
    auto plainShader = shadersBucket.plainShader.get();
    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
    glVertexAttribPointer(plainShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, pointVertices.data());
    glUniform1f(plainShader->getPointSizeLocation(), 30.0f);
    glUniform4f(plainShader->getColorLocation(), 1.0f, 0.0f, 0.0f, 1.0f);
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glDrawArrays(GL_POINTS, 0, 1);

    std::vector<float> axisLatitudeVertices = {
            0, 0, 0,
            static_cast<float>(axisLatitude.x() * radius * 1.3f),
            static_cast<float>(axisLatitude.y() * radius * 1.3),
            static_cast<float>(axisLatitude.z() * radius * 1.3f)
    };
    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
    glVertexAttribPointer(plainShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, axisLatitudeVertices.data());
    glUniform1f(plainShader->getPointSizeLocation(), 30.0f);
    glLineWidth(8.0f);
    glUniform4f(plainShader->getColorLocation(), 1.0f, 0.0f, 0.0f, 1.0f);
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glDrawArrays(GL_LINES, 0, 2);


    std::vector<float> axisLongitudeVertices = {
            0, 0, 0,
            static_cast<float>(axisLongitude.x() * radius * 1.3f),
            static_cast<float>(axisLongitude.y() * radius * 1.3),
            static_cast<float>(axisLongitude.z() * radius * 1.3f)
    };
    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pv.data());
    glVertexAttribPointer(plainShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, axisLongitudeVertices.data());
    glUniform1f(plainShader->getPointSizeLocation(), 30.0f);
    glLineWidth(8.0f);
    glUniform4f(plainShader->getColorLocation(), 0.0f, 1.0f, 0.0f, 1.0f);
    glEnableVertexAttribArray(plainShader->getPosLocation());
    glDrawArrays(GL_LINES, 0, 2);
}

void UserMarker::setPosition(float lat, float lon) {
    this->latitude = lat;
    this->longitude = lon;
}

void UserMarker::clearTexture() {
    glDeleteTextures(1, &textureId);
}

void UserMarker::setTexture(GLuint texture) {
    this->textureId = texture;
}
