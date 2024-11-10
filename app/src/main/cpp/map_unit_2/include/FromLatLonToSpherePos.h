//
// Created by Artem on 10.11.2024.
//

#ifndef TUSA_ANDROID_FROMLATLONTOSPHEREPOS_H
#define TUSA_ANDROID_FROMLATLONTOSPHEREPOS_H

#include <Eigen/Dense>
#include "MapNumbers.h"


class FromLatLonToSpherePos {
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

    Eigen::Vector3d getPoint(MapNumbers& mapNumbers, float latitude, float longitude) {
        auto radius = mapNumbers.radius;
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


#endif //TUSA_ANDROID_FROMLATLONTOSPHEREPOS_H
