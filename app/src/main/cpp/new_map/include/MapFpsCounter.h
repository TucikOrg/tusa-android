//
// Created by Artem on 17.09.2024.
//

#ifndef TUSA_ANDROID_MAPFPSCOUNTER_H
#define TUSA_ANDROID_MAPFPSCOUNTER_H

#include <chrono>

class MapFpsCounter {
public:
    float newFrame();

    float getFps() {
        return fps;
    }

    float getTimeElapsed() {
        return timeElapsed.count();
    }
private:
    std::chrono::high_resolution_clock::time_point previousRenderFrameTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point fpsRenderFrameTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point updateFrameStateTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    int frameCount;
    float fps;
    std::chrono::duration<float> timeElapsed;
};


#endif //TUSA_ANDROID_MAPFPSCOUNTER_H
