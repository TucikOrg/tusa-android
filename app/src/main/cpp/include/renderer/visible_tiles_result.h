//
// Created by Artem on 01.08.2024.
//

#ifndef TUSA_ANDROID_VISIBLE_TILES_RESULT_H
#define TUSA_ANDROID_VISIBLE_TILES_RESULT_H

#include <vector>
#include "renderer/visible_tiles_block.h"
#include "tile_cords.h"
#include <map>

class VisibleTilesResult {
public:
    VisibleTilesBlock visibleBlocks;
    std::map<std::string, TileCords> newVisibleTiles = {};
    short renderZDiffSize;
    short renderYDiffSize;
};


#endif //TUSA_ANDROID_VISIBLE_TILES_RESULT_H
