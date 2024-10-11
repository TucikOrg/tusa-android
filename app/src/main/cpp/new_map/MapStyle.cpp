//
// Created by Artem on 17.09.2024.
//

#include "MapStyle.h"

#include "util/android_log.h"

struct StringExtractor : public boost::static_visitor<std::string> {
    std::string operator()(const std::string& str) const {
        return str;
    }

    template<typename T>
    std::string operator()(const T& /*unused*/) const {
        return ""; // Or throw an exception if necessary
    }
};

class TypeVisitor : public boost::static_visitor<std::string> {
public:
    template <typename T>
    std::string operator()(const T&) const {
        return typeid(T).name();
    }
};

bool MapStyle::registerAdminLayer(
        std::string layerName,
        uint64_t adminLevel,
        int& currentIndex
) {
    if(layerName == "admin" && adminLevel == 0) {
        lineWidth[currentIndex] = 2.0f;
        color[currentIndex] = CSSColorParser::parse("rgba(94, 0, 172, 1.0)");
        visibleZoom[currentIndex] = allZoomsVisible({{0, nullptr}});
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if(layerName == "admin" && adminLevel == 1) {
        lineWidth[currentIndex] = 1.0f;
        color[currentIndex] = CSSColorParser::parse("rgba(94, 0, 172, 1.0)");
        visibleZoom[currentIndex] = allZoomsVisible({{0, nullptr}});
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if(layerName == "admin" && adminLevel == 2) {
        lineWidth[currentIndex] = 0.1f;
        color[currentIndex] = CSSColorParser::parse("rgba(94, 0, 172, 1.0)");
        visibleZoom[currentIndex] = allZoomsVisible({{0, nullptr}});
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    return false;
}

bool MapStyle::registerLandUseLayer(std::string layerName, std::string className, int& currentIndex) {
    auto zooms = fromToZoomsVisible(14, 22);
    if (layerName == "landuse" && className == "wood") {
        color[currentIndex] = CSSColorParser::parse("rgba(194, 239, 179, 0.8)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "landuse" && className == "scrub") {
        color[currentIndex] = CSSColorParser::parse("rgba(224, 246, 213, 0.6)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "landuse" && className == "agriculture") {
        color[currentIndex] = CSSColorParser::parse("rgba(243, 252, 238, 0.6)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "landuse" && className == "park") {
        color[currentIndex] = CSSColorParser::parse("rgb(192, 240, 168)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "landuse" && className == "grass") {
        color[currentIndex] = CSSColorParser::parse("rgba(243, 252, 238, 0.6)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "landuse" && className == "airport") {
        color[currentIndex] = CSSColorParser::parse("rgb(244, 243, 251)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "landuse" && className == "cemetery") {
        color[currentIndex] = CSSColorParser::parse("rgb(210, 236, 198)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "landuse" && className == "glacier") {
        color[currentIndex] = CSSColorParser::parse("rgb(204, 233, 245)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "landuse" && className == "hospital") {
        color[currentIndex] = CSSColorParser::parse("rgb(246, 238, 244)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "landuse" && className == "pitch") {
        color[currentIndex] = CSSColorParser::parse("rgb(217, 248, 186)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "landuse" && className == "sand") {
        color[currentIndex] = CSSColorParser::parse("rgb(236, 251, 228)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "landuse" && className == "school") {
        color[currentIndex] = CSSColorParser::parse("rgb(241, 236, 228)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "landuse" && className == "parking") {
        color[currentIndex] = CSSColorParser::parse("rgb(245, 245, 245)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "landuse" && className == "residential") {
        color[currentIndex] = CSSColorParser::parse("rgb(255, 255, 255)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "landuse" && (className == "facility" || className == "industrial")) {
        color[currentIndex] = CSSColorParser::parse("rgb(255, 255, 255)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "landuse" && className == "commercial_area") {
        color[currentIndex] = CSSColorParser::parse("rgb(252, 244, 232)");
        visibleZoom[currentIndex] = zooms;
        alphaInterpolateFrom[currentIndex] = 14.0f;
        alphaInterpolateTo[currentIndex] = 14.5f;
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

//    if (layerName == "landuse") {
//        color[currentIndex] = CSSColorParser::parse("rgb(242, 242, 242)");
//        visibleZoom[currentIndex] = zooms;
//        styles.insert(currentIndex);
//        return true;
//    } else currentIndex++;

    return false;
}

bool MapStyle::registerRoadLayer(std::string layerName, std::string className, int& currentIndex) {
    if (layerName == "road" && className == "primary") {
        lineWidth[currentIndex] = 2.0;
        color[currentIndex] = CSSColorParser::parse("rgb(233, 233, 237)");
        visibleZoom[currentIndex] = allZoomsVisible({});
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "road" && className == "motorway") {
        lineWidth[currentIndex] = 2.0;
        color[currentIndex] = CSSColorParser::parse("rgb(249, 199, 128)");
        visibleZoom[currentIndex] = allZoomsVisible({});
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "road" && className == "trunk") {
        lineWidth[currentIndex] = 2.0f;
        color[currentIndex] = CSSColorParser::parse("rgb(240, 227, 86)");
        visibleZoom[currentIndex] = allZoomsVisible({});
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if (layerName == "road" && (className == "secondary" || className == "tertiary")) {
        forwardRenderingOnly[currentIndex] = true;
        isWideLine[currentIndex] = true;
        lineWidth[currentIndex] = 4.0f;
        borderFactor[currentIndex] = 0.0f;
        color[currentIndex] = CSSColorParser::parse("rgb(233, 233, 237)");
        visibleZoom[currentIndex] = allZoomsVisible({});
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    // fallback
//    if (layerName == "road") {
//        lineWidth[currentIndex] = 1.0;
//        color[currentIndex] = CSSColorParser::parse("rgba(254, 229, 154, 1.0)");
//        visibleZoom[currentIndex] = allZoomsVisible({});
//        styles.insert(currentIndex);
//        return true;
//    } else currentIndex++;

    return false;
}

std::unordered_map<short, void*> MapStyle::allZoomsVisible(std::unordered_map<short, void*> expect) {
    std::unordered_map<short, void*> zoomsMap;
    for (short zoom = 0; zoom < 20; ++zoom) {
        if (expect.find(zoom) != expect.end()) {
            continue;
        }
        zoomsMap.insert({zoom, expect[zoom]});
    }
    return zoomsMap;
}

unsigned short MapStyle::determineStyle(std::string layerName, layer_map_type props, int z) {
    std::vector<std::string> ignoreLayers;
    ignoreLayers.push_back("hillshade");
    ignoreLayers.push_back("natural_label");
    ignoreLayers.push_back("place_label");
    if(std::find(ignoreLayers.begin(), ignoreLayers.end(), layerName) != ignoreLayers.end()) {
        return 0;
    }

    int currentIndex = 1;
    std::string className;
    for(auto prop : props) {
        if(prop.first == "class") {
            className = boost::get<std::string>(prop.second);
        }
    }

    uint64_t adminLevel = 0;
    if(layerName == "admin") {
        auto adminIso = props["iso_3166_1"];
        auto adminLevelVariant = props["admin_level"];
        std::string iso3166Admin = boost::apply_visitor(StringExtractor(), adminIso);
        adminLevel = boost::get<uint64_t>(adminLevelVariant);
    }

    if (layerName == "landcover") {
        color[currentIndex] = CSSColorParser::parse("rgba(103, 234, 44, 0.4f)");
        visibleZoom[currentIndex] = allZoomsVisible({});
        styles.insert(currentIndex);
        return currentIndex;
    } else currentIndex++;

    if (layerName == "landuse_overlay" && className == "national_park") {
        color[currentIndex] = CSSColorParser::parse("rgba(71, 169, 94, 0.4)");
        visibleZoom[currentIndex] = allZoomsVisible({});
        styles.insert(currentIndex);
        return currentIndex;
    } else currentIndex++;

    if (registerLandUseLayer(layerName, className, currentIndex)) {
        return currentIndex;
    }

    if (layerName == "water") {
        color[currentIndex] = CSSColorParser::parse("rgba(0, 186, 255, 1.0)");
        visibleZoom[currentIndex] = allZoomsVisible({});
        styles.insert(currentIndex);
        return currentIndex;
    } else currentIndex++;

    if (registerRoadLayer(layerName, className, currentIndex)) {
        return currentIndex;
    }

    if (registerAdminLayer(layerName, adminLevel, currentIndex)) {
        return currentIndex;
    }

    if (layerName == "building") {
        color[currentIndex] = CSSColorParser::parse("rgb(237, 237, 237)");
        visibleZoom[currentIndex] = fromToZoomsVisible(15, 22);
        alphaInterpolateFrom[currentIndex] = 15.0f;
        alphaInterpolateTo[currentIndex] = 16.0f;
        styles.insert(currentIndex);
        return currentIndex;
    } else currentIndex++;


    LOGI("Unknown style for layer %s class %s", layerName.c_str(), className.c_str());

    return 0;
}

std::unordered_map<short, void *> MapStyle::fromToZoomsVisible(int from, int to) {
    std::unordered_map<short, void*> zoomsMap;
    for (int i = from; i <= to; ++i) {
        zoomsMap.insert({i, nullptr});
    }
    return zoomsMap;
}
