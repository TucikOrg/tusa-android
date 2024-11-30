//
// Created by Artem on 30.11.2024.
//

#ifndef TUSA_ANDROID_DRAWTITLESTHREADINPUT_H
#define TUSA_ANDROID_DRAWTITLESTHREADINPUT_H

#include <map>
#include "MapTile.h"

class DrawTitlesThreadInput {
public:
    std::unordered_map<uint64_t, MapTile*> tiles;

    bool recalculate = false;
    bool ready = true;
};


#endif //TUSA_ANDROID_DRAWTITLESTHREADINPUT_H
