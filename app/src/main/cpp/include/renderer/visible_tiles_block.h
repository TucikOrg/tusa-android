//
// Created by Artem on 01.08.2024.
//

#ifndef TUSA_ANDROID_VISIBLE_TILES_BLOCK_H
#define TUSA_ANDROID_VISIBLE_TILES_BLOCK_H


class VisibleTilesBlock {
public:
    float tileX_start_f;
    float tileX_end_f;
    float tileY_start_f;
    float tileY_end_f;

    int tileX_start;
    int tileX_end;
    int tileY_start;
    int tileY_end;
    int zoom;
};


#endif //TUSA_ANDROID_VISIBLE_TILES_BLOCK_H
