//
// Created by Artem on 16.10.2024.
//

#ifndef TUSA_ANDROID_SMOOTHMAPTEXTURERENDER_H
#define TUSA_ANDROID_SMOOTHMAPTEXTURERENDER_H

#include "MapTile.h"

class SmoothMapTextureRender {
public:
    void nextFrame() {
        if (allReady == true) {
            frameCount = -1;
        }
        allReady = true;
        frameCount++;
    }
    bool isStart() {
        return frameCount == 0;
    }

    bool getAllReady() {
        return allReady;
    }

    bool isReady(MapTile* tile) {
        auto size = tile->style.getStylesVec().size();
        bool ready = renderSize + frameCount > size;
        if (ready == false) {
            allReady = false;
        }
        return ready;
    }

    unsigned short getFrom() {
        return 0 + frameCount;
    }

    unsigned short getTo() {
        return renderSize + frameCount;
    }


private:
    int frameCount = -1;
    bool allReady = true;
    int renderSize = 1;
};


#endif //TUSA_ANDROID_SMOOTHMAPTEXTURERENDER_H
