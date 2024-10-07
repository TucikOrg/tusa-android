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
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if(layerName == "admin" && adminLevel == 1) {
        lineWidth[currentIndex] = 1.0f;
        color[currentIndex] = CSSColorParser::parse("rgba(94, 0, 172, 1.0)");
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    if(layerName == "admin" && adminLevel == 2) {
        lineWidth[currentIndex] = 0.5f;
        color[currentIndex] = CSSColorParser::parse("rgba(94, 0, 172, 1.0)");
        styles.insert(currentIndex);
        return true;
    } else currentIndex++;

    return false;
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

    if (className == "landuse") {
        color[currentIndex] = CSSColorParser::parse("rgba(242, 242, 242, 0.5)");
        styles.insert(currentIndex);
        return currentIndex;
    } else currentIndex++;

    if (layerName == "landcover") {
        color[currentIndex] = CSSColorParser::parse("rgba(103, 234, 44, 0.4f)");
        styles.insert(currentIndex);
        return currentIndex;
    } else currentIndex++;


    if (layerName == "water") {
        color[currentIndex] = CSSColorParser::parse("rgba(0, 186, 255, 1.0)");
        styles.insert(currentIndex);
        return currentIndex;
    } else currentIndex++;

    if (layerName == "road") {
        lineWidth[currentIndex] = 1.0;
        color[currentIndex] = CSSColorParser::parse("rgba(254, 229, 154, 1.0)");
        styles.insert(currentIndex);
        return currentIndex;
    } else currentIndex++;

    if (registerAdminLayer(layerName, adminLevel, currentIndex)) {
        return currentIndex;
    }

    LOGI("Unknown style for layer %s class %s", layerName.c_str(), className.c_str());

    return 0;
}
