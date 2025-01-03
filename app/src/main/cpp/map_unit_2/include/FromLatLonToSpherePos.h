//
// Created by Artem on 10.11.2024.
//

#ifndef TUSA_ANDROID_FROMLATLONTOSPHEREPOS_H
#define TUSA_ANDROID_FROMLATLONTOSPHEREPOS_H

#include <Eigen/Dense>
#include "MapNumbers.h"
#include "IGetPointData.h"

class FromLatLonToSphereDoublePos {
public:
    void init(MapNumbers& mapNumbers) {
        double angleLongitude = -1.0 * mapNumbers.EPSG3857LonNormInf * M_PI;
        double angleLatitude = mapNumbers.EPSG4326CamLat;

        Eigen::AngleAxisd rotationPlanetLatitude = Eigen::AngleAxisd(angleLatitude, Eigen::Vector3d(1, 0, 0));
        Eigen::AngleAxisd rotationPlanetLongitude = Eigen::AngleAxisd(angleLongitude, rotationPlanetLatitude * Eigen::Vector3d(0, 1, 0));
        axisLatitude  = rotationPlanetLongitude * Eigen::Vector3d(1, 0, 0);
        axisLongitude = rotationPlanetLatitude * Eigen::Vector3d(0, 1, 0);
        pointOnSphere = rotationPlanetLongitude * rotationPlanetLatitude * Eigen::Vector3d(0, 0, 1);
    }

    Eigen::Vector3d getPoint(float radius, float latitude, float longitude) {
        Eigen::AngleAxisd rotationLatitude = Eigen::AngleAxisd(-latitude, axisLatitude);
        Eigen::AngleAxisd rotationLongitude = Eigen::AngleAxisd(longitude, axisLongitude);
        Eigen::Vector3d markerDirectionSphere = rotationLongitude * rotationLatitude * pointOnSphere;
        Eigen::Vector3d markerPointLocation = markerDirectionSphere * radius;

        float markerX = markerPointLocation.x();
        float markerY = markerPointLocation.y();
        float markerZ = markerPointLocation.z() - radius;
        return { markerX, markerY, markerZ };
    }

    Eigen::Vector3d pointOnSphere;
    Eigen::Vector3d axisLatitude;
    Eigen::Vector3d axisLongitude;
};

class FromLatLonToSpherePos {
private:
    float axisLatitude_XY;
    float axisLatitude_XX;
    float axisLatitude_XZ;
    float axisLatitude_YZ;
    float axisLatitude_ZZ;
    float axisLatitude_YY;
    
    float axisLongitude_XY;
    float axisLongitude_XX;
    float axisLongitude_XZ;
    float axisLongitude_YZ;
    float axisLongitude_ZZ;
    float axisLongitude_YY;

public:
    void init(float EPSG3857LonNormInf, float EPSG4326CamLat) {
        float angleLongitude = -1.0 * EPSG3857LonNormInf * M_PI;
        float angleLatitude = EPSG4326CamLat;

        Eigen::AngleAxis rotationPlanetLatitude = Eigen::AngleAxis(angleLatitude, Eigen::Vector3f(1, 0, 0));
        Eigen::AngleAxis rotationPlanetLongitude = Eigen::AngleAxis(angleLongitude, rotationPlanetLatitude * Eigen::Vector3f(0, 1, 0));
        axisLatitude  = rotationPlanetLongitude * Eigen::Vector3f(1, 0, 0);
        axisLongitude = rotationPlanetLatitude * Eigen::Vector3f(0, 1, 0);
        pointOnSphere = rotationPlanetLongitude * rotationPlanetLatitude * Eigen::Vector3f(0, 0, 1);

        axisLatitude_XY = axisLatitude.x() * axisLatitude.y();
        axisLatitude_XX = axisLatitude.x() * axisLatitude.x();
        axisLatitude_XZ = axisLatitude.x() * axisLatitude.z();
        axisLatitude_YZ = axisLatitude.y() * axisLatitude.z();
        axisLatitude_ZZ = axisLatitude.z() * axisLatitude.z();
        axisLatitude_YY = axisLatitude.y() * axisLatitude.y();

        axisLongitude_XY = axisLongitude.x() * axisLongitude.y();
        axisLongitude_XX = axisLongitude.x() * axisLongitude.x();
        axisLongitude_XZ = axisLongitude.x() * axisLongitude.z();
        axisLongitude_YZ = axisLongitude.y() * axisLongitude.z();
        axisLongitude_ZZ = axisLongitude.z() * axisLongitude.z();
        axisLongitude_YY = axisLongitude.y() * axisLongitude.y();
    }

    Eigen::Vector3f getPoint(float radius, IGetPointData& markerMapTitle) {
        Eigen::Matrix3f rotationLatitude;
        rotationLatitude << markerMapTitle.cos_minus_lat + axisLatitude_XX * (1.0 - markerMapTitle.cos_minus_lat),
                axisLatitude_XY * (1.0 - markerMapTitle.cos_minus_lat) - axisLatitude.z() * markerMapTitle.sin_minus_lat,
                axisLatitude_XZ * (1.0 - markerMapTitle.cos_minus_lat) + axisLatitude.y() * markerMapTitle.sin_minus_lat,

                axisLatitude_XY * (1.0 - markerMapTitle.cos_minus_lat) + axisLatitude.z() * markerMapTitle.sin_minus_lat,
                markerMapTitle.cos_minus_lat + axisLatitude_YY * (1.0 - markerMapTitle.cos_minus_lat),
                axisLatitude_YZ * (1.0 - markerMapTitle.cos_minus_lat) - axisLatitude.x() * markerMapTitle.sin_minus_lat,

                axisLatitude_XZ * (1.0 - markerMapTitle.cos_minus_lat) - axisLatitude.y() * markerMapTitle.sin_minus_lat,
                axisLatitude_YZ * (1.0 - markerMapTitle.cos_minus_lat) + axisLatitude.x() * markerMapTitle.sin_minus_lat,
                markerMapTitle.cos_minus_lat + axisLatitude_ZZ * (1.0 - markerMapTitle.cos_minus_lat);

        
        Eigen::Matrix3f rotationLongitude;
        rotationLongitude << markerMapTitle.cos_lon + axisLongitude_XX * (1.0 - markerMapTitle.cos_lon),
                axisLongitude_XY * (1.0 - markerMapTitle.cos_lon) - axisLongitude.z() * markerMapTitle.sin_lon,
                axisLongitude_XZ * (1.0 - markerMapTitle.cos_lon) + axisLongitude.y() * markerMapTitle.sin_lon,

                axisLongitude_XY * (1.0 - markerMapTitle.cos_lon) + axisLongitude.z() * markerMapTitle.sin_lon,
                markerMapTitle.cos_lon + axisLongitude_YY * (1.0 - markerMapTitle.cos_lon),
                axisLongitude_YZ * (1.0 - markerMapTitle.cos_lon) - axisLongitude.x() * markerMapTitle.sin_lon,

                axisLongitude_XZ * (1.0 - markerMapTitle.cos_lon) - axisLongitude.y() * markerMapTitle.sin_lon,
                axisLongitude_YZ * (1.0 - markerMapTitle.cos_lon) + axisLongitude.x() * markerMapTitle.sin_lon,
                markerMapTitle.cos_lon + axisLongitude_ZZ * (1.0 - markerMapTitle.cos_lon);

        Eigen::Vector3f point = (rotationLongitude * rotationLatitude * pointOnSphere) * radius;

        return { point.x(), point.y(), point.z() - radius };
    }

    Eigen::Vector3f pointOnSphere;
    Eigen::Vector3f axisLatitude;
    Eigen::Vector3f axisLongitude;
};


#endif //TUSA_ANDROID_FROMLATLONTOSPHEREPOS_H
