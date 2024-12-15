//
// Created by Artem on 17.10.2024.
//

#include "UserMarker.h"

UserMarker::UserMarker(unsigned char* pixels, float latitude, float longitude, int64_t markerId)
    : pixels(pixels), latitude(latitude), longitude(longitude), markerId(markerId) {
}

void UserMarker::setPosition(float lat, float lon) {
    this->latitude = lat;
    this->longitude = lon;
}


