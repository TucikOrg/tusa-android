//
// Created by Artem on 17.09.2024.
//

#include "MapControls.h"

void MapControls::drag(float dx, float dy) {
    screenXDragged += dx;
    screenYDragged += dy;
}
