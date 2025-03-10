//
// Created by Artem on 16.09.2024.
//

#ifndef TUSA_ANDROID_MAPTILERENDER_H
#define TUSA_ANDROID_MAPTILERENDER_H

#include "csscolorparser/csscolorparser.h"
#include "shader/shaders_bucket.h"
#include "MapTile.h"
#include "MapTest.h"
#include "MapCamera.h"
#include "MapStyle.h"
#include "MapTileGetter.h"
#include "TileAndMatrix.h"
#include "util/eigen_gl.h"
#include "MapTest.h"
#include "RenderTextureData.h"
#include "SmoothMapTextureRender.h"
#include "DrawSymbol.h"
#include <GLES2/gl2.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/StdVector>
#include "Grid.h"
#include "RoadLettersPtr.h"

class MapTileRender {
public:
    MapTileRender();

    void initTilesTexture();

    void renderTile(
            ShadersBucket& shadersBucket,
            MapTile* tile,
            Eigen::Matrix4f p,
            Eigen::Matrix4f vm,
            Eigen::Matrix4f pvm,
            float zoom,
            bool isForwardRendering,
            unsigned short from = 0,
            unsigned short to = 0
    );

    void renderTexture(
            RenderTextureData data,
            MapCamera& mapCamera,
            ShadersBucket& shadersBucket
    );

    void drawLayer(
            ShadersBucket& shadersBucket,
            MapTile* tile,
            Eigen::Matrix4f p,
            Eigen::Matrix4f vm,
            Eigen::Matrix4f pvm,
            int styleIndex,
            float zoom,
            bool isForwardRendering
    );

    void drawBackground(
            ShadersBucket& shadersBucket,
            Eigen::Matrix4f pvm
    );

    float getScaleText(MapNumbers& mapNumbers);

    void renderPathText(MapTile* tile, MapSymbols& mapSymbols,
                        Eigen::Matrix4f vm, Eigen::Matrix4f p,
                        ShadersBucket& shadersBucket, MapNumbers& mapNumbers,
                        float elapsedTime, Eigen::Matrix4f pvm,  float scaleText
    );

    GLuint getTilesTexture();
    GLuint getMapTexture() {
        return mapTexture;
    }

    void destroy();


    std::thread parallelThread;
    bool parallelThreadRunning = true;
    bool lockThread = true;
    void joinThreads() {
        parallelThreadRunning = false;
        if (parallelThread.joinable()) {
            parallelThread.join();
        }
    }
    int savedVisTileYStart, savedVisTileYEnd, savedVisTileXStartInf, savedVisTileXEndInf, savedN, savedTileZ;
    std::unordered_map<uint64_t, MapTile*> savedTiles;
    float textCollisionDelta = -25;
    Eigen::Matrix4f savedPVScreen;

    float screenWidth, screenHeight;

    void resetLettersRoadCollision() {
        roadLettersPtr.clear();
        roadLettersScreenXY.clear();
    }
    void shouldRecalculateCollisions(float elapsedTime) {
        float timeFromLastCollisionCheck = elapsedTime - elapsedTimeLastCollisionCheck;
        checkCollisions = timeFromLastCollisionCheck > 2; // каждые две секунды считаем коллизии между текстом
        if (checkCollisions) {
            elapsedTimeLastCollisionCheck = elapsedTime;
        }
    }
    void setCheckRoadsCollisionsFalse() {
        checkCollisions = false;
    }
private:
    bool checkCollisions = true;
    std::vector<RoadLettersPtr> roadLettersPtr;
    std::vector<float> roadLettersScreenXY;

    float extent = 4096;
    GLuint tilesTexture;
    GLuint tilesFrameBuffer;
    int prTex2dHeight;
    int prTex2dWidth;
    GLuint mapTexture;
    GLuint mapTextureFramebuffer;

    float elapsedTimeLastCollisionCheck = 0;

};


#endif //TUSA_ANDROID_MAPTILERENDER_H
