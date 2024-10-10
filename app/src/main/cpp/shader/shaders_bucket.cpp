//
// Created by Artem on 20.12.2023.
//

#include "shader/shaders_bucket.h"
#include <iostream>
#include <fstream>
#include <string>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>


ShadersBucket::ShadersBucket() {}

void ShadersBucket::compileAllShaders(AAssetManager* assetManager) {
    plainShader = std::shared_ptr<PlainShader>(new PlainShader(
            assetManager, "shaders/plain.vert", "shaders/plain.frag"
    ));
    symbolShader = std::shared_ptr<SymbolShader>(new SymbolShader(
            assetManager, "shaders/symbol.vert", "shaders/symbol.frag"
    ));
    planetShader = std::shared_ptr<PlanetShader>(new PlanetShader(
            assetManager, "shaders/planet.vert", "shaders/planet.frag"
    ));
    planet2Shader = std::shared_ptr<Planet2Shader>(new Planet2Shader(
            assetManager, "shaders/planet2.vert", "shaders/planet2.frag"
    ));
    starsShader = std::shared_ptr<StarsShader>(new StarsShader(
            assetManager, "shaders/stars.vert", "shaders/stars.frag"
    ));
    planetGlowShader = std::shared_ptr<PlanetGlowShader>(new PlanetGlowShader(
            assetManager, "shaders/planet_glow.vert", "shaders/planet_glow.frag"
    ));
    textureShader = std::shared_ptr<TextureShader>(new TextureShader(
            assetManager, "shaders/texture.vert", "shaders/texture.frag"
    ));
    userMarkerShader = std::shared_ptr<UserMarkerShader>(new UserMarkerShader(
            assetManager, "shaders/user_marker.vert", "shaders/user_marker.frag"
    ));
    planet3Shader = std::shared_ptr<Planet3Shader>(new Planet3Shader(
            assetManager, "shaders/planet3.vert", "shaders/planet3.frag"
    ));
    planet3FRendShader = std::shared_ptr<Planet3FRendShader>(new Planet3FRendShader(
            assetManager, "shaders/planet3_f_rend.vert", "shaders/planet3_f_rend.frag"
    ));
    roadShader = std::shared_ptr<RoadShader>(new RoadShader(
            assetManager, "shaders/road.vert", "shaders/road.frag"
    ));
    simplePointShader = std::shared_ptr<SimplePointShader>(new SimplePointShader(
            assetManager, "shaders/simple_point.vert", "shaders/simple_point.frag"
    ));
    squarePointShader = std::shared_ptr<SquarePointShader>(new SquarePointShader(
            assetManager, "shaders/square_point.vert", "shaders/square_point.frag"
    ));
}

ShadersBucket::~ShadersBucket() {

}






