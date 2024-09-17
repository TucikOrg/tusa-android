//
// Created by Artem on 17.09.2024.
//

#include "MapStyle.h"

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

unsigned short MapStyle::determineStyle(std::string layerName, layer_map_type props) {
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
        color[currentIndex] = CSSColorParser::parse("rgb(242, 242, 242)");
        styles.insert(currentIndex);
        return currentIndex;
    } else currentIndex++;

    if (layerName == "landcover") {
        color[currentIndex] = CSSColorParser::parse("rgba(204, 244, 185, 1.0)");
        styles.insert(currentIndex);
        return currentIndex;
    } else currentIndex++;


    if (layerName == "water") {
        lineWidth[currentIndex] = 8;
        color[currentIndex] = CSSColorParser::parse("rgb(0, 186, 255)");
        styles.insert(currentIndex);
        return currentIndex;
    } else currentIndex++;

    if (layerName == "road") {
        lineWidth[currentIndex] = 8;
        color[currentIndex] = CSSColorParser::parse("rgb(254, 229, 154)");
        styles.insert(currentIndex);
        return currentIndex;
    } else currentIndex++;

    if(layerName == "admin") {
        lineWidth[currentIndex] = 5;
        color[currentIndex] = CSSColorParser::parse("rgb(94, 0, 172)");
        styles.insert(currentIndex);
        return currentIndex;
    } else currentIndex++;

    return 0;
}
