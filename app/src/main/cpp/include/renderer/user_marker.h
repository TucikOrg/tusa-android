//
// Created by Artem on 08.08.2024.
//

#ifndef TUSA_ANDROID_USER_MARKER_H
#define TUSA_ANDROID_USER_MARKER_H

#include <Eigen/Geometry>
#include <Eigen/Core>
#include <Eigen/StdVector>
#include <Eigen/Dense>
#include "shader/user_marker_shader.h"
#include "util/eigen_gl.h"
#include "util/android_log.h"

class UserMarker {

public:
    UserMarker() = default;
    UserMarker(
            float latitude,
            float longitude,
            float planetRadius,
            bool flatRender,
            float markerSize,
            unsigned int textureId
            ) :
    latitude(latitude),
    longitude(longitude),
    planetRadius(planetRadius),
    textureId(textureId) {
        moveTo(latitude, longitude, flatRender);
        float initScale = 1.0;
        animateToScale(initScale);
        scale(initScale);
        generateGeometry(markerSize);
    }

    void onChangeRenderMode(bool flatRender) {
        moveTo(latitude, longitude, flatRender);
    }

    void moveTo(float latitude, float longitude, bool flatRender) {
        this->latitude = latitude;
        this->longitude = longitude;

        auto point = Point {
            0, 0, 0
        };
        if (!flatRender) {
            point = CommonUtils::planetCordsToWorldCords(latitude, longitude, planetRadius);
        } else {
            float y = CommonUtils::latitudeToFlatCameraY(latitude, planetRadius);
            float z = CommonUtils::longitudeToFlatCameraZ(longitude, planetRadius);
            point = Point {0, y, z};
        }

        this->x = point.x;
        this->y = point.y;
        this->z = point.z;
    }

    void generateGeometry(float size) {
        float height = 2 * size + size / 2;

        // bottom left
        vertices[0] = 0;
        vertices[1] = 0;
        vertices[2] = 0 + size;

        // bottom right
        vertices[3] = 0;
        vertices[4] = 0;
        vertices[5] = 0 - size;

        // top right
        vertices[6] = 0;
        vertices[7] = 0 + height;
        vertices[8] = 0 - size;

        // top left
        vertices[9] = 0;
        vertices[10] = 0 + height;
        vertices[11] = 0 + size;
    }

    void animateToScale(float scale) {
        this->targetScale = scale;
    }

    void scale(float scale) {
        currentAnimScale = scale;
        const int dim = 4;
        Eigen::Vector4f scale_factors(scale, scale, scale, 1.0f);
        Eigen::DiagonalMatrix<float, dim> scaling_matrix(scale_factors);
        scaleMatrix = scaling_matrix.toDenseMatrix();
    }

    void draw(Eigen::Matrix4f pvm, std::shared_ptr<UserMarkerShader> userMarkerShader,
              float camLongitude, float camLatitude,
              float planetLongitude, float planetLatitude,
              Eigen::Vector3f cameraPosition, bool flatRender
              ) {

        if (currentAnimScale != targetScale) {
            float delta = targetScale - currentAnimScale;
            float step = delta / 10;
            currentAnimScale += step;
            scale(currentAnimScale);
        }


        float longitudeDelta = abs(longitude - planetLongitude);
        float latitudeDelta = abs(latitude - planetLatitude);
        bool enableDepthTest = longitudeDelta > M_PI / 3 || latitudeDelta > M_PI / 3;
        if (planetLatitude > DEG2RAD(80) && latitude > 0) {
            enableDepthTest = false;
        }
        if (planetLatitude < -DEG2RAD(80) && latitude < 0) {
            enableDepthTest = false;
        }
        if (enableDepthTest) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        UserMarkerShader *shader = userMarkerShader.get();
        glUseProgram(shader->program);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glUniform1f(shader->getTextureLocation(), 0);

        Eigen::Vector3f cameraXZ = Eigen::Vector3f(cameraPosition.x(), 0, cameraPosition.z());
        cameraXZ.normalize();
        Eigen::Vector3f axisLatitude = cameraXZ.cross(Eigen::Vector3f(0, 1, 0));

        Eigen::Matrix4f rotationLongitudeMatrix = EigenGL::createRotationMatrixAxis(-camLongitude, Eigen::Vector3f(0, 1, 0));
        Eigen::Matrix4f rotationLatitudeMatrix = EigenGL::createRotationMatrixAxis(camLatitude, axisLatitude);

        Eigen::Matrix4f translationMatrix = EigenGL::createTranslationMatrix(x, y, z);
        Eigen::Matrix4f usePvm = Eigen::Matrix4f::Identity();
        if (flatRender) {
            usePvm = pvm * translationMatrix * scaleMatrix;
        } else {
            usePvm = pvm * translationMatrix * rotationLatitudeMatrix * rotationLongitudeMatrix * scaleMatrix;
        }
        glUniformMatrix4fv(shader->getMatrixLocation(), 1, GL_FALSE, usePvm.data());
        glVertexAttribPointer(shader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, textureCords);
        glEnableVertexAttribArray(shader->getTextureCord());
        glVertexAttribPointer(shader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, vertices);
        glEnableVertexAttribArray(shader->getPosLocation());
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
    }

    float getLatitude() const {
        return latitude;
    }

    float getLongitude() const {
        return longitude;
    }

    unsigned int getTextureId() const {
        return textureId;
    }

    void setTextureId(unsigned int textureId) {
        this->textureId = textureId;
    }

private:
    GLuint textureId;
    Eigen::Matrix4f scaleMatrix;

    float currentAnimScale;
    float targetScale;

    float planetRadius;
    float latitude;
    float longitude;
    float x;
    float y;
    float z;
    float vertices[12] = {
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f
    };
    GLfloat textureCords[8] = {
            0.0f, 1.25f,
            1.0f, 1.25f,
            1.0f, 0.0f,
            0.0f, 0.0f
    };
    unsigned int indices[6] = {
            0, 1, 2,
            2, 3, 0
    };
};


#endif //TUSA_ANDROID_USER_MARKER_H
