//
// Created by Artem on 20.12.2023.
//

#ifndef TUSA_SHADERS_BUCKET_H
#define TUSA_SHADERS_BUCKET_H

#include <map>
#include <android/asset_manager.h>
#include "plain_shader.h"
#include "symbol_shader.h"
#include "shaders_bucket.h"
#include "planet_shader.h"
#include "stars_shader.h"
#include "planet_glow_shader.h"
#include "texture_shader.h"
#include "user_marker_shader.h"
#include "planet2_shader.h"
#include "planet3_shader.h"
#include "planet3_f_rend.h"
#include "road_shader.h"
#include "SimplePointShader.h"
#include "SquarePointShader.h"
#include <boost/shared_ptr.hpp>

class ShadersBucket {
public:
    ShadersBucket();
    ~ShadersBucket();

    void compileAllShaders(AAssetManager* assetManager);

    std::shared_ptr<RoadShader> roadShader;
    std::shared_ptr<PlainShader> plainShader;
    std::shared_ptr<SymbolShader> symbolShader;
    std::shared_ptr<PlanetShader> planetShader;
    std::shared_ptr<Planet2Shader> planet2Shader;
    std::shared_ptr<StarsShader> starsShader;
    std::shared_ptr<PlanetGlowShader> planetGlowShader;
    std::shared_ptr<TextureShader> textureShader;
    std::shared_ptr<UserMarkerShader> userMarkerShader;
    std::shared_ptr<Planet3Shader> planet3Shader;
    std::shared_ptr<Planet3FRendShader> planet3FRendShader;
    std::shared_ptr<SimplePointShader> simplePointShader;
    std::shared_ptr<SquarePointShader> squarePointShader;
};


#endif //TUSA_SHADERS_BUCKET_H
