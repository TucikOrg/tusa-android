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
    ~Renderer() = default;
private:
    // Camera functions
    float evaluateCameraDistance(float _scaleFactor, float zoomDiff);
    float evaluateCameraDistanceCurrentZ(short zoomDiff = 0) {
        return evaluateCameraDistance(realMapZTile(), zoomDiff);
    }
    [[nodiscard]] float getCamLongitude() const {
        return cameraLongitudeRad;
    }
    [[nodiscard]] float getCamLatitude() const {
        return cameraLatitudeRad;
    }
    [[nodiscard]] Eigen::Vector3f getCameraPosition() const {
        return {camX, camY, camZ};
    }
    void updateCameraPosition();
    void updateFrustum();
    CornersCords evaluateCorners(Eigen::Matrix4f pvm);

    // Surface, Space environment functions
    void drawStars(Eigen::Matrix4f pvm);
    void drawGlowing(Eigen::Matrix4f pvm);
    void generateStarsGeometry();

    // Map scale factor, extents
    float evaluateScaleFactor(float z, float zoomDiff = 0);
    float evaluateScaleFactorCurrentZ(short zoomDiff = 0);
    short realMapZTile() { return (short) scaleFactorZoom; }
    short currentMapZTile();
    float evaluateExtentForScaleFactor(float scaleFactor);
    float evaluateCurrentExtent();
    void updateMapZoomScaleFactor(float scaleFactor) { scaleFactorZoom = scaleFactor; }

    // Open Gl functions, core functions
public:
    void onSurfaceChanged(int w, int h);
    void onSurfaceCreated(AAssetManager *assetManager);
    void renderFrame();
    void setupNoOpenGLMapState(float scaleFactor, AAssetManager *assetManager, JNIEnv *env);
    void onStop();
private:
    Eigen::Matrix4f evaluatePVM();
    void clearColor();
    void loadAssets(AAssetManager *assetManager);
    void loadTextures(AAssetManager *assetManager);
    void switchFlatAndSphereRender();
    void drawPoints(Eigen::Matrix4f matrix, std::vector<float> points, float pointSize);
    void drawPoint(Eigen::Matrix4f matrix, float x, float y, float z, float pointSize);
    void glViewPortDefaultSize() { glViewport(0, 0, screenW, screenH); }
    GLuint loadTextureFromAsset(const char* assetName, AAssetManager *assetManager);
    GLuint loadTextureFromBytes(unsigned char* imageData, off_t fileSize);

    // Planet map
    void drawPlanet(Eigen::Matrix4f);
    void updatePlanetGeometry(VisibleTilesResult visibleTilesResult);
    float getSphereLonRad();
    float getSphereLatRad() { return cameraLatitudeRad; }
    float getPlanetCurrentLongitudeDeg() { return RAD2DEG(getSphereLonRad()); }
    float getPlanetCurrentLatitudeDeg() { return RAD2DEG(getSphereLatRad()); }
    void evaluateLatLonByIntersectionPlanes_Sphere(Eigen::Vector4f firstPlane, Eigen::Vector4f secondPlane, Eigen::Matrix4f pvm, double& longitudeRad, double& latitudeRad, bool& has);

    // Flat map
    float flatZNormalized(float z);
    float flatYNormalized(float y);
    float getFlatLongitude();
    float getFlatLatitude();
    void drawFlat(Eigen::Matrix4f);
    void evaluateLatLonByIntersectionPlanes_Flat(Eigen::Vector4f firstPlane, Eigen::Vector4f secondPlane, Eigen::Matrix4f pvm, float& x, float& y);

    // Tiles
    VisibleTilesResult updateVisibleTilesSphere(CornersCords cornersCords);
    VisibleTilesResult updateVisibleTilesFlat(CornersCords cornersCords);
    void networkTilesFunction(JavaVM* gJvm, GetTileRequest* getTileRequest);
    void networkRootTileFunction(JavaVM* gJvm, GetTileRequest* getTileRequest);
    void pushToNetworkTiles(std::map<std::string, TileCords>& newVisibleTiles);
    void updateRenderTileProjection(short amountX, short amountY);
    void renderTiles(std::vector<TileCords> renderTiles, Eigen::Matrix4f pvmTexture);

    // Controls
public:
    void drag(float dx, float dy);
    void scale(float scaleFactor);
    void doubleTap();
    void onDown();
private:

    // Markers
public:
    void handleMarker(std::string key, float latitude, float longitude, unsigned char *imageData, off_t fileSize);
    void addMarker(std::string key, float latitude, float longitude, unsigned char* imageData, off_t fileSize);
    void updateMarkerGeo(std::string key, float latitude, float longitude);
    void updateMarkerAvatar(std::string key, unsigned char* imageData, off_t fileSize);
    void removeMarker(std::string key);
private:
    float evaluateInitMarkerSize() {  return planetRadius * markerSizePlanetPortion; }
    void markersOnChangeRenderMode();
    void updateMarkersSizes();

    // Test UI
    Eigen::Matrix4f evaluatePVM_UI();
    void drawFPS_UI(Eigen::Matrix4f pvmUI);
    void drawTilesRenderTexture_UI(Eigen::Matrix4f pvmUI);

private:
    // Changed parameters
    short drawStarsToZoom = 3;
    short networkTilesThreads = 3;
    short mapZTileCordMax = 19;
    float planetRadius = 1000000;
    float cameraScaleOneUnitSphere = 5; // число больше, планета меньше
    //float cameraScaleOneUnitFlat = 2.9; // число больше, плоскость меньше
    float dragCameraSpeedFlatMap = 1000;
    float forceCameraMoveOnDragSphere = 0.01;
    float forceCameraMoveOnDragFlat = 5000;
    float refreshRenderDataEverySeconds = 0.5;
    short switchSphere2FlatAtZoom = 15;
    float markerSizePlanetPortion = 0.13;
    float fovy = 80.0f;

    CSSColorParser::Color spaceColor = CSSColorParser::parse("rgb(1, 21, 64)");
    CSSColorParser::Color backgroundColor = CSSColorParser::parse("rgb(255, 255, 255)");

    float fps;
    std::chrono::high_resolution_clock::time_point previousRenderFrameTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point fpsRenderFrameTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point updateFrameStateTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;

    bool flatRender = false;
    std::shared_ptr<Symbols> getSymbols() {return symbols;}

    std::vector<float> starsVertices = {};
    std::vector<float> starsSizes = {};

    CornersCords currentCornersCords;
    std::map<std::string, UserMarker> userMarkers = {};
    std::stack<TileCords> networkTilesStack;
    TileNode* readyTilesTree;
    std::vector<std::thread*> networkTileThreads;
    short _savedLastScaleStateMapZ;

    bool switchFlatSphereModeFlag = false;
    unsigned int defaultAvatarTextureId;
    VisibleTilesResult visibleTilesResult;
    std::map<std::string, RenderTileHash> renderTileHash;
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
    TilesStorage tilesStorage = TilesStorage(cache);
    GLuint renderMapTexture;
    GLuint renderMapFrameBuffer;
    std::string currentVisibleTilesKey;
    // текущее положение камеры в пространстве
    float camX = 0;
    float camY = 0;
    float camZ = 0;
    float flatTileSizeInit = 2.0f * planetRadius * (M_PI / 2);
    float flatHalfOfTileSizeInit = flatTileSizeInit / 2;

};


#endif //TUSA_RENDERER_H
