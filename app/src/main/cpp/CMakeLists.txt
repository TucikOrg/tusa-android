# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level freetype.cmake, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("tusaandroid")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level freetype.cmake, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's freetype.cmake, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.
add_library(${CMAKE_PROJECT_NAME} SHARED
        # List C/C++ source files with relative paths to this freetype.cmake.
        native-lib.cpp

        shader/plain_shader.cpp
        shader/symbol_shader.cpp
        shader/shaders_bucket.cpp
        shader/shader.cpp
        shader/planet_shader.cpp
        shader/planet2_shader.cpp
        shader/stars_shader.cpp
        shader/planet_glow_shader.cpp
        shader/texture_shader.cpp
        shader/user_marker_shader.cpp
        shader/planet3_shader.cpp
        shader/planet3_f_rend.cpp
        shader/road_shader.cpp
        shader/space_dark_shader.cpp

        util/timer.cpp
        util/eigen_gl.cpp

        map/linestring_handler.cpp
        map/polygon_handler.cpp

        csscolorparser/csscolorparser.cpp

        map_unit_1/MapCamera.cpp
        map_unit_1/MapTest.cpp
        map_unit_1/MapTileRender.cpp
        map_unit_1/MapTile.cpp
        map_unit_1/MapPolygon.cpp
        map_unit_1/MapSimpleLine.cpp
        map_unit_1/MapPolygonAggregated.cpp
        map_unit_1/MapTileGetter.cpp
        map_unit_1/MapStyle.cpp
        map_unit_1/MapFpsCounter.cpp
        map_unit_1/MapSymbols.cpp
        map_unit_1/PointHandler.cpp

        map_unit_2/MapRenderer.cpp
        map_unit_2/MapControls.cpp
        map_unit_2/TileAndMatrix.cpp
        map_unit_2/MapWideLine.cpp
        map_unit_2/MapSimplePoint.cpp
        map_unit_2/SimplePointShader.cpp
        map_unit_2/SquarePointShader.cpp
        map_unit_2/MapSquarePoints.cpp
        map_unit_2/CircularBuffer.cpp
        map_unit_2/MapColors.cpp
        map_unit_2/MapEnvironment.cpp
        map_unit_2/RenderTextureData.cpp
        map_unit_2/DrawMap.cpp
        map_unit_2/DrawMapData.cpp
        map_unit_2/MapNumbers.cpp
        map_unit_2/SmoothMapTextureRender.cpp
        map_unit_2/Markers.cpp
        map_unit_2/UserMarker.cpp
        map_unit_2/TextureUtils.cpp
        map_unit_2/Symbol.cpp
        map_unit_2/FromLatLonToSpherePos.cpp
        map_unit_2/AnimateCameraTo.cpp
        map_unit_2/MarkerMapTitle.cpp
        map_unit_2/TextTexture.cpp
        map_unit_2/DrawTextAlongPath.cpp
        map_unit_2/TransferTilesToGPU.cpp
        map_unit_2/SymbolInAtlas.cpp
        map_unit_2/PrepareCharAtlas.cpp
        map_unit_2/ByLayersDraw.cpp
)

add_subdirectory(${PROJECT_SOURCE_DIR}/eigen)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE include)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE map_unit_1/include)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE map_unit_2/include)

include(${PROJECT_SOURCE_DIR}/vendor/protozero.cmake)
include(${PROJECT_SOURCE_DIR}/vendor/earcut.hpp.cmake)
include(${PROJECT_SOURCE_DIR}/vendor/boost.cmake)
include(${PROJECT_SOURCE_DIR}/vendor/variant.cmake)
include(${PROJECT_SOURCE_DIR}/vendor/vtzero.cmake)
include(${PROJECT_SOURCE_DIR}/vendor/freetype.cmake)

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
        mbgl-vendor-protozero
        mbgl-vendor-earcut.hpp
        mbgl-vendor-boost
        mbgl-vendor-variant
        vtzero-vendor
        freetype
        Eigen3::Eigen
        )

target_link_libraries(${CMAKE_PROJECT_NAME} log)
target_link_libraries(${CMAKE_PROJECT_NAME} GLESv2)
target_link_libraries(${CMAKE_PROJECT_NAME} android)
target_link_libraries(${CMAKE_PROJECT_NAME} z)
