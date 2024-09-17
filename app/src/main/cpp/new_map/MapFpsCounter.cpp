//
// Created by Artem on 17.09.2024.
//

#include "MapFpsCounter.h"
#include "util/android_log.h"

float MapFpsCounter::newFrame() {
    auto currentTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> frameDeltaTime = currentTime - previousRenderFrameTime;
    previousRenderFrameTime = currentTime;
    frameCount++;
    std::chrono::duration<float> fpsTimeSpan = currentTime - fpsRenderFrameTime;
    if (fpsTimeSpan.count() >= 1.0f) {
        fps = frameCount / fpsTimeSpan.count();
        frameCount = 0;
        fpsRenderFrameTime = currentTime;
        LOGI("FPS: %f", fps);
    }
    timeElapsed = currentTime - startTime;

    return frameDeltaTime.count();
}
