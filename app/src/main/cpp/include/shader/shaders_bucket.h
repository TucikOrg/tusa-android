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
#include "space_dark_shader.h"
#include "road_shader.h"
#include "SimplePointShader.h"
#include "SquarePointShader.h"
#include "title_map_shader.h"
#include "path_text_shader.h"
#include "avatar_on_map_new_shader.h"
#include "avatar_ray_shader.h"
#include "avatars_on_map_big_zoom_shader.h"
#include <boost/shared_ptr.hpp>

class ShadersBucket {
public:
    ShadersBucket();
    ~ShadersBucket();

    void compileAllShaders(AAssetManager* assetManager);
    void destroy();

    std::shared_ptr<RoadShader> roadShader;
    std::shared_ptr<PlainShader> plainShader;
    std::shared_ptr<SymbolShader> symbolShader;
    std::shared_ptr<PlanetShader> planetShader;
    std::shared_ptr<Planet2Shader> planet2Shader;
    std::shared_ptr<StarsShader> starsShader;
    std::shared_ptr<PlanetGlowShader> planetGlowShader;
    std::shared_ptr<SpaceDarkShader> spaceDarkShader;
    std::shared_ptr<TextureShader> textureShader;
    std::shared_ptr<UserMarkerShader> userMarkerShader;
    std::shared_ptr<Planet3Shader> planet3Shader;
    std::shared_ptr<SimplePointShader> simplePointShader;
    std::shared_ptr<SquarePointShader> squarePointShader;
    std::shared_ptr<TitleMapShader> titlesMapShader;
    std::shared_ptr<PathTextShader> pathTextShader;
    std::shared_ptr<AvatarRayShader> avatarRayShader;
    std::shared_ptr<AvatarOnMapNewShader> avatarOnMapNewShader;
    std::shared_ptr<AvatarsOnMapBigZoomShader> avatarOnMapNewBigZoomShader;
};


#endif //TUSA_SHADERS_BUCKET_H
