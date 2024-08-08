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
    float refreshRenderDataEverySeconds = 0.5;
    short switchFlatSphereOnZoom = 15;
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

    void loadTextures(AAssetManager *assetManager);

private:
    float fps;
    std::vector<float> starsVertices = {};
    std::vector<float> starsSizes = {};
    CornersCords currentCornersCords;
    std::map<std::string, TileCords> previousVisibleTiles = {};
    std::vector<UserMarker> userMarkers = {};

    std::stack<TileCords> networkTilesStack;
    TileNode* readyTilesTree;
    std::vector<std::thread*> networkTileThreads;
    float _savedLastScaleStateMapZ;
    std::chrono::high_resolution_clock::time_point previousRenderFrameTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point fpsRenderFrameTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point updateFrameStateTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    bool switchFlatSphereModeFlag = false;

    unsigned int testTextureId;
    unsigned int testAvatarTextureId;

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

    void renderTiles(std::vector<TileCords> renderTiles, Eigen::Matrix4f pvmTexture) {
        std::shared_ptr<PlainShader> plainShader = shadersBucket->plainShader;

        int leftX = visibleTilesResult.visibleBlocks.tileX_start;
        int leftY = visibleTilesResult.visibleBlocks.tileY_start;
        int z = visibleTilesResult.visibleBlocks.zoom;
        int rightX = visibleTilesResult.visibleBlocks.tileX_end;

        glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glUseProgram(plainShader->program);

        for(short geometryHeapIndex = Style::maxGeometryHeaps; geometryHeapIndex >= 0; --geometryHeapIndex) {
            for(auto& tileCords : renderTiles) {
                auto& visibleTile = tileCords;
                auto visibleTileKey = tileCords.getKey();

                Eigen::Matrix4f forTileMatrix;
                if (renderTileHash.find(visibleTileKey) != renderTileHash.end()) {
                    short deltaTileZ = z - visibleTile.getTileZ();

                    int visibleTileN = pow(2, visibleTile.getTileZ());
                    int topLefTileN = pow(2, z);

                    double visibleTileX = (double)visibleTile.getTileX() / visibleTileN;
                    double visibleTileY = (double)visibleTile.getTileY() / visibleTileN;

                    double topLeftTileX = (double)leftX / topLefTileN;
                    double topLeftTileY = (double)leftY / topLefTileN;

                    double deltaTileXPlanet = visibleTileX - topLeftTileX;
                    double deltaTileYPlanet = visibleTileY - topLeftTileY;

                    bool isVisibleTileOnRightSideOfEdge =
                            leftX > rightX &&
                            visibleTileX < topLeftTileX;
                    if (isVisibleTileOnRightSideOfEdge) {
                        deltaTileXPlanet += 1;
                    }

                    // либо больше текущего зума, либо меньше текущего зума.
                    // насколько он больше или меньше относительно текущего зума
                    float scaleVisual2topLeft = pow(2, deltaTileZ);
                    // размер относительно текущего видимого тайла
                    // extent это всегда одна доля рендеринга тексутры всех тайлов
                    // поэтому умножая на скейл получаем относительный размер
                    float sizeOfVisualTile = extent * scaleVisual2topLeft;

                    double tilesXDelta = deltaTileXPlanet * visibleTileN;
                    double tilesYDelta = deltaTileYPlanet * visibleTileN;
                    float deltaVisualX = tilesXDelta * sizeOfVisualTile;
                    float deltaVisualY = -tilesYDelta * sizeOfVisualTile;

                    Eigen::Affine3f modelTranslation(Eigen::Translation3f(deltaVisualX,deltaVisualY, 0));
                    Eigen::Affine3f scaleMatrix(Eigen::Scaling(scaleVisual2topLeft, scaleVisual2topLeft, 0.0f));
                    forTileMatrix = pvmTexture * modelTranslation.matrix() * scaleMatrix.matrix();

                    renderTileHash[visibleTileKey] = RenderTileHash { forTileMatrix };
                } else {
                    forTileMatrix = renderTileHash[visibleTileKey].forTileMatrix;
                }

                glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, forTileMatrix.data());
                Tile* tile = visibleTile.getDrawGeometry();

                // рисуем бекграунд
                if(geometryHeapIndex == Style::maxGeometryHeaps) {
                    CSSColorParser::Color colorOfStyle = CSSColorParser::parse("rgb(241, 255, 230)");
                    GLfloat red   = static_cast<GLfloat>(colorOfStyle.r) / 255;
                    GLfloat green = static_cast<GLfloat>(colorOfStyle.g) / 255;
                    GLfloat blue  = static_cast<GLfloat>(colorOfStyle.b) / 255;
                    GLfloat alpha = static_cast<GLfloat>(colorOfStyle.a);
                    const GLfloat color[] = { red, green, blue, alpha};
                    glUniform4fv(plainShader->getColorLocation(), 1, color);

                    float backPoints[] = {
                            0, 0,
                            (float) extent, 0,
                            (float) extent, -(float) extent,
                            0, 0,
                            0, - (float) extent,
                            (float) extent, -(float) extent,
                    };

                    glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT,
                                          GL_FALSE, 0, backPoints
                    );
                    glEnableVertexAttribArray(plainShader->getPosLocation());
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                    continue;
                }

                // Рисуем основную карту
                if(geometryHeapIndex < Style::maxGeometryHeaps) {
                    Geometry<float, unsigned int>& polygonsGeometry = tile->resultPolygons[geometryHeapIndex];
                    Geometry<float, unsigned int>& linesGeometry = tile->resultLines[geometryHeapIndex];
                    if(polygonsGeometry.isEmpty() && linesGeometry.isEmpty())
                        continue;

                    float lineWidth = tile->style.getLineWidthOfHeap(geometryHeapIndex);
                    glLineWidth(lineWidth);

                    CSSColorParser::Color colorOfStyle = tile->style.getColorOfGeometryHeap(geometryHeapIndex);
                    GLfloat red   = static_cast<GLfloat>(colorOfStyle.r) / 255;
                    GLfloat green = static_cast<GLfloat>(colorOfStyle.g) / 255;
                    GLfloat blue  = static_cast<GLfloat>(colorOfStyle.b) / 255;
                    GLfloat alpha = static_cast<GLfloat>(colorOfStyle.a);
                    const GLfloat color[] = { red, green, blue, alpha};
                    glUniform4fv(plainShader->getColorLocation(), 1, color);


                    if (!polygonsGeometry.isEmpty()) {
                        glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT,
                                              GL_FALSE, 0, polygonsGeometry.points
                        );
                        glEnableVertexAttribArray(plainShader->getPosLocation());
                        glDrawElements(GL_TRIANGLES, polygonsGeometry.indicesCount, GL_UNSIGNED_INT, polygonsGeometry.indices);
                    }

                    if (!linesGeometry.isEmpty()) {
                        glVertexAttribPointer(plainShader->getPosLocation(), 2, GL_FLOAT,
                                              GL_FALSE, 0, linesGeometry.points
                        );
                        glEnableVertexAttribArray(plainShader->getPosLocation());
                        glDrawElements(GL_LINES, linesGeometry.indicesCount, GL_UNSIGNED_INT, linesGeometry.indices);
                    }

                    continue;
                }
            }
        }
    }

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
    double longitudeCameraMoveCurrentForce = 0;
    double latitudeCameraMoveCurrentForce = 0;
    double latitudeCameraAngleRadConstraint = DEG2RAD(85);
    double cameraLatitudeRad = 0, cameraLongitudeRad = 0;
    float cameraZ = 0, cameraY = 0;
    int screenW, screenH;
    int renderMapTextureWidth, renderMapTextureHeight;
    Cache* cache;
    TilesStorage tilesStorage = TilesStorage(cache, nullptr);
    std::map<std::string, TileCords> tilesForRenderer = {};
    GLuint renderMapTexture;
    GLuint renderMapFrameBuffer;
    std::string renderFrameKey = "";

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
    void loadTextureFromAsset(unsigned int& textureId, const char* assetName, AAssetManager *assetManager);

};


#endif //TUSA_RENDERER_H
