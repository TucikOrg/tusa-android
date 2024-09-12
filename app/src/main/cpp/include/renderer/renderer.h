//
// Created by Artem on 02.01.2024.
//

#ifndef TUSA_RENDERER_H
#define TUSA_RENDERER_H

#include "shader/shaders_bucket.h"
#include "map/tile.h"
#include "map/tiles_storage.h"
#include "style/style.h"
#include "visible_tile_render_mode.h"
#include "symbols/symbols.h"
#include "render_tile_coordinates.h"
#include "render_tiles_enum.h"
#include "tile_cords.h"
#include "util/android_log.h"
#include "geometry/sphere.h"
#include "gl/triangle_example.h"
#include "center_borders_cords.h"
#include "corners_cords.h"
#include "util/eigen_gl.h"
#include "tile_node.h"
#include "render_tile_hash.h"
#include "visible_tiles_result.h"
#include "user_marker.h"
#include "point.h"
#include <vector>
#include <Eigen/Core>
#include <Eigen/StdVector>
#include <Eigen/Dense>
#include <queue>
#include <stack>

class Renderer {
public:
    Renderer(std::shared_ptr<ShadersBucket> shadersBucket, Cache* cache);
    ~Renderer() {}

    // изменяемые параметры
    short drawStarsToZoom = 3;
    short networkTilesThreads = 3;
    short mapZTileCordMax = 19;
    float planetRadius = 1000000;
    float cameraScaleOneUnitSphere = 5; // число больше, планета меньше
    float cameraScaleOneUnitFlat = 2.9; // число больше, плоскость меньше
    float dragCameraSpeedFlatMap = 1000;
    float forceCameraMoveOnDragSphere = 0.01;
    float forceCameraMoveOnDragFlat = 5000;
    float refreshRenderDataEverySeconds = 0.5;
    short switchFlatSphereOnZoom = 15;
    float markerSizePlanetPortion = 0.13;
    float fovy = 80.0f;
    CSSColorParser::Color spaceColor = CSSColorParser::parse("rgb(1, 21, 64)");
    CSSColorParser::Color backgroundColor = CSSColorParser::parse("rgb(255, 255, 255)");
    bool flatRender = false;

    void generateStarsGeometry();
    void drawStars(Eigen::Matrix4f pvm);
    void drawGlowing(Eigen::Matrix4f pvm);
    void clearColor();
    void onSurfaceChanged(int w, int h);
    void onSurfaceCreated(AAssetManager *assetManager);
    void renderFrame();
    VisibleTilesResult updateVisibleTilesSphere(CornersCords cornersCords);
    VisibleTilesResult updateVisibleTilesFlat(CornersCords cornersCords);
    void drag(float dx, float dy);
    void scale(float scaleFactor);
    void doubleTap();
    void onDown();
    void networkTilesFunction(JavaVM* gJvm, GetTileRequest* getTileRequest);
    void setupNoOpenGLMapState(float scaleFactor, AAssetManager *assetManager, JNIEnv *env);

    void handleMarker(std::string key, float latitude, float longitude, unsigned char *imageData, off_t fileSize);
    void addMarker(std::string key, float latitude, float longitude, unsigned char* imageData, off_t fileSize);
    void updateMarkerGeo(std::string key, float latitude, float longitude);
    void updateMarkerAvatar(std::string key, unsigned char* imageData, off_t fileSize);
    void removeMarker(std::string key);

    Eigen::Vector3f getCameraPosition() {
        return Eigen::Vector3f(camX, camY, camZ);
    }
    float getCamLongitude() {
        return cameraLongitudeRad;
    }
    float getCamLatitude() {
        return cameraLatitudeRad;
    }

    float evaluateCameraDistance(float _scaleFactor, bool flatRender, float zoomDiff) {
        float scale = evaluateScaleFactor(_scaleFactor, zoomDiff); // от 1 до cameraScaleOneUnitSphere * 2^19

        float resultDistance = 0;
        if (flatRender) {
            float zoomingDistance = cameraScaleOneUnitFlat * scale;
            resultDistance = zoomingDistance;
        } else {
            float zoomingDistance = cameraScaleOneUnitSphere * scale;
            resultDistance = zoomingDistance + planetRadius;
        }
        LOGI("Camera distance %f", resultDistance);
        return resultDistance;
    }

    float evaluateCameraDistanceCurrentZ(short zoomDiff = 0) {
        return evaluateCameraDistance(realMapZTile(), flatRender, zoomDiff);
    }

    void updateCameraPosition(bool flatRender) {
        cameraCurrentDistance = evaluateCameraDistance(scaleFactorZoom, flatRender, 0);
        LOGI("Camera distance %f", cameraCurrentDistance);
    }

    float evaluateScaleFactor(float z, float zoomDiff = 0) {
        return pow(2, mapZTileCordMax - z - zoomDiff);
    }

    float evaluateScaleFactorCurrentZ(short zoomDiff = 0) {
        return evaluateScaleFactor(realMapZTile(), zoomDiff);
    }

    short realMapZTile() {
        return (short) scaleFactorZoom;
    }

    short currentMapZTile() {
        short realZ = realMapZTile();
        if (realZ > 16)
            realZ = 16;
        return realZ;
    }

    float evaluateExtentForScaleFactor(float scaleFactor) {
        return (float) extent * scaleFactor;
    }

    float evaluateCurrentExtent() {
        return evaluateExtentForScaleFactor(evaluateScaleFactorCurrentZ());
    }

    void updatePlanetGeometry(VisibleTilesResult visibleTilesResult);

    std::shared_ptr<Symbols> getSymbols() {return symbols;}

    void drawPlanet(Eigen::Matrix4f);
    void drawFlat(Eigen::Matrix4f);
    void onStop();
    void loadAssets(AAssetManager *assetManager);

    void updateMapZoomScaleFactor(float scaleFactor) {
        scaleFactorZoom = scaleFactor;
    }

    float evaluateInitMarkerSize() {
        return planetRadius * markerSizePlanetPortion;
    }

    void markersOnChangeRenderMode() {
        for(auto& marker : userMarkers) {
            marker.second.onChangeRenderMode(flatRender);
        }
    }

    void updateMarkersSizes() {
        float newScale = 1 / pow(2, realMapZTile());
        if (flatRender) {
            newScale *= 0.85;
        }
        for(auto& marker : userMarkers) {
            marker.second.animateToScale(newScale);
        }
    }

    void loadTextures(AAssetManager *assetManager);

private:
    int count = 0;

    float fps;
    std::vector<float> starsVertices = {};
    std::vector<float> starsSizes = {};
    CornersCords currentCornersCords;
    std::map<std::string, TileCords> previousVisibleTiles = {};
    std::map<std::string, UserMarker> userMarkers = {};

    std::stack<TileCords> networkTilesStack;
    TileNode* readyTilesTree;
    std::vector<std::thread*> networkTileThreads;
    float _savedLastScaleStateMapZ;
    std::chrono::high_resolution_clock::time_point previousRenderFrameTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point fpsRenderFrameTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point updateFrameStateTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    bool switchFlatSphereModeFlag = false;

    unsigned int defaultAvatarTextureId;

    void updateRenderTileProjection(short amountX, short amountY);

    void updateFrustum(bool flatRender);

    float flatZNormalized(float z) {
        return fmod(z, planetRadius) - planetRadius;
    }

    float flatYNormalized(float y) {
        return fmod(y, planetRadius) - planetRadius;
    }

    float getFlatLongitude() {
        return -(cameraZ / planetRadius) * M_PI;
    }

    float getFlatLatitude() {
        float y = ((cameraY / planetRadius) + 1) / 2;
        return CommonUtils::tileLatitude(1 - y, 1);
    }

    void switchFlatAndSphereRender() {
        short z = currentMapZTile();
        int n = pow(2, z);

        if (flatRender) {
            float flatLongitude = getFlatLongitude();
            float flatLatitude = getFlatLatitude();

            cameraLongitudeRad = -flatLongitude;
            cameraLatitudeRad = flatLatitude;
            LOGI("FLAT -> SPHERE lat(%f) lon(%f)", flatLatitude, flatLongitude);
        } else {
            // это сфера и хотим плоскость
            float longitudeRad = getSphereLonRad();
            float latitudeRad = getSphereLatRad();
            cameraZ = CommonUtils::longitudeToFlatCameraZ(longitudeRad, planetRadius);
            cameraY = CommonUtils::latitudeToFlatCameraY(latitudeRad, planetRadius);
            LOGI("SPHERE -> FLAT lat(%f) lon(%f)", latitudeRad, longitudeRad);
        }

        switchFlatSphereModeFlag = false;
        flatRender = !flatRender;

        updateCameraPosition(flatRender);
        updateFrustum(flatRender);
        //LOGI("Flat lon %f lat %f", RAD2DEG(flatLongitude), RAD2DEG(flatLatitude));
    }

    VisibleTilesResult visibleTilesResult;
    std::map<std::string, RenderTileHash> renderTileHash;

    void renderTiles(std::vector<TileCords> renderTiles, Eigen::Matrix4f pvmTexture);

    Eigen::Matrix4f evaluatePVM();

    float getSphereLonRad() {
        return CommonUtils::normalizeLongitudeRad(-cameraLongitudeRad);
    }

    float getSphereLatRad() {
        return cameraLatitudeRad;
    }

    float getPlanetCurrentLongitudeDeg() {
        return RAD2DEG(getSphereLonRad());
    }

    float getPlanetCurrentLatitudeDeg() {
        return RAD2DEG(getSphereLatRad());
    }

    CornersCords evaluateCorners(Eigen::Matrix4f pvm);
    void evaluateLatLonByIntersectionPlanes_Sphere(Eigen::Vector4f firstPlane, Eigen::Vector4f secondPlane, Eigen::Matrix4f pvm, double& longitudeRad, double& latitudeRad, bool& has);

    void evaluateLatLonByIntersectionPlanes_Flat(Eigen::Vector4f firstPlane, Eigen::Vector4f secondPlane, Eigen::Matrix4f pvm, float& x, float& y);


    void drawPoints(Eigen::Matrix4f matrix, std::vector<float> points, float pointSize) {
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);
        auto plainShader = shadersBucket.get()->plainShader;
        const GLfloat color[] = { 1, 0, 0, 1};
        glUseProgram(plainShader->program);
        glUniform1f(plainShader->getPointSizeLocation(), pointSize);
        glUniform4fv(plainShader->getColorLocation(), 1, color);
        glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, matrix.data());
        glVertexAttribPointer(plainShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, points.data());
        glEnableVertexAttribArray(plainShader->getPosLocation());
        glDrawArrays(GL_POINTS, 0, points.size() / 3);
    }

    void drawPoint(Eigen::Matrix4f matrix, float x, float y, float z, float pointSize) {
        float points[] = {x, y, z};
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);
        auto plainShader = shadersBucket.get()->plainShader;
        const GLfloat color[] = { 1, 0, 0, 1};
        glUseProgram(plainShader->program);
        glUniform1f(plainShader->getPointSizeLocation(), pointSize);
        glUniform4fv(plainShader->getColorLocation(), 1, color);
        glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, matrix.data());
        glVertexAttribPointer(plainShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, points);
        glEnableVertexAttribArray(plainShader->getPosLocation());
        glDrawArrays(GL_POINTS, 0, 1);
    }

    bool isInPreviousVisibleTiles(TileCords& tileCords) {
        return previousVisibleTiles.find(tileCords.getKey()) != previousVisibleTiles.end();
    }

    void glViewPortDefaultSize() {
        glViewport(0, 0, screenW, screenH);
    }

    std::shared_ptr<ShadersBucket> shadersBucket;
    std::shared_ptr<RenderTileCoordinates> renderTileCoordinates;
    std::shared_ptr<Symbols> symbols;

    Eigen::Matrix4f projectionMatrix;
    Eigen::Matrix4f rendererTileProjectionMatrix;


    const uint32_t extent = 4096;
    Sphere planetGeometry = Sphere();
    float cameraCurrentDistance = 0;
    float scaleFactorZoom = 0.0f; // из MapView устанавливается
    float longitudeCameraMoveCurrentForce = 0;
    float latitudeCameraMoveCurrentForce = 0;
    float latitudeCameraAngleRadConstraint = DEG2RAD(85);
    float cameraLatitudeRad = 0, cameraLongitudeRad = 0;
    float cameraZ = 0, cameraY = 0;

    int screenW, screenH;
    int renderMapTextureWidth, renderMapTextureHeight;
    Cache* cache;
    TilesStorage tilesStorage = TilesStorage(cache, nullptr);
    GLuint renderMapTexture;
    GLuint renderMapFrameBuffer;
    std::string currentVisibleTilesKey = "";

    // текущее положение камеры в пространстве
    float camX = 0;
    float camY = 0;
    float camZ = 0;

    Eigen::Matrix4f evaluatePVM_UI() {
        Eigen::Vector3f cameraPosition = Eigen::Vector3f(0, 0, 1);
        Eigen::Vector3f target(0.0f, 0.0f, 0.0f);
        Eigen::Vector3f up(0.0f, 1.0f, 0.0f);
        Eigen::Matrix4f viewMatrixUI = EigenGL::createViewMatrix(cameraPosition, target, up);
        Eigen::Matrix4f projectionMatrixUI = EigenGL::createOrthoMatrix(0, screenW, 0, screenH, 0.1f, 1.0f);
        Eigen::Matrix4f pvmUI = projectionMatrixUI * viewMatrixUI;
        return pvmUI;
    }

    void drawFPS_UI(Eigen::Matrix4f pvmUI) {
        glStencilMask(0X00);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);

        std::shared_ptr<SymbolShader> symbolShader = shadersBucket->symbolShader;
        CSSColorParser::Color color = CSSColorParser::parse("rgb(255, 255, 255)");
        symbols.get()->renderText("FPS: " + std::to_string((short)fps), 200, screenH - 100, pvmUI, 2, color);
    }

    void drawTilesRenderTexture_UI(Eigen::Matrix4f pvmUI) {
        std::shared_ptr<TextureShader> textureShader = shadersBucket->textureShader;
        glUseProgram(textureShader->program);
        glStencilMask(0X00);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);

        float sizeUI = screenW / 4;
        float points[] = {
                0, sizeUI,
                0, 0,
                sizeUI, 0,
                sizeUI, sizeUI
        };
        GLfloat textureCords[] = {
                0.0f, 1.0f,
                0.0f, 0.0f,
                1.0f, 0.0f,
                1.0f, 1.0f
        };
        unsigned int indices[6] = {
                2, 3, 0,
                0, 1, 2
        };
        glUniformMatrix4fv(textureShader->getMatrixLocation(), 1, GL_FALSE, pvmUI.data());
        glBindTexture(GL_TEXTURE_2D, renderMapTexture);
        glVertexAttribPointer(textureShader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, textureCords);
        glEnableVertexAttribArray(textureShader->getTextureCord());
        glVertexAttribPointer(textureShader->getPosLocation(), 2, GL_FLOAT, GL_FALSE, 0, points);
        glEnableVertexAttribArray(textureShader->getPosLocation());
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
    }

    GLuint loadTextureFromAsset(const char* assetName, AAssetManager *assetManager);

    GLuint loadTextureFromBytes(unsigned char* imageData, off_t fileSize);
};


#endif //TUSA_RENDERER_H
