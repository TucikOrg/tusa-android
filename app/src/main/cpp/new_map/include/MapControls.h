//
// Created by Artem on 17.09.2024.
//

#ifndef TUSA_ANDROID_MAPCONTROLS_H
#define TUSA_ANDROID_MAPCONTROLS_H

#include "map/mercator.h"

class MapControls {
public:
    void drag(float dx, float dy);

    float getScreenXDragged() {
        return screenXDragged;
    }

    float getScreenYDragged() {
        return screenYDragged;
    }

    float geTextureUShift() {
        return screenXDragged * 0.00015f;
    }

    float getCameraLatitudeRad() {
        float rad = -screenYDragged * 0.001f;
        float limit = DEG2RAD(85);
        if (rad > limit) {
            rad = limit;
        } else if (rad < -limit) {
            rad = -limit;
        }
        return rad;
    }

private:
    float screenXDragged = 0;
    float screenYDragged = 0;
};


#endif //TUSA_ANDROID_MAPCONTROLS_H
