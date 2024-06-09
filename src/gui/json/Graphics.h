#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace JX11::Gui::JsonData
{
inline static const std::string kGraphicsFileName { "graphics.json" };

inline static const std::string kFrameType { "frame" };
inline static const std::string kTextType { "text" };

enum class GraphicsType {
    eFrame,
    eText,
};

NLOHMANN_JSON_SERIALIZE_ENUM(GraphicsType,
                             {
                                 { GraphicsType::eFrame, kFrameType },
                                 { GraphicsType::eText, kTextType },
                             })

struct Frame
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    // int borderRadius = 0;
    int thickness = 1;
    std::string color = "black";
    // std::string bgColor = "white";
};

struct Text
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    std::string text;
    std::string color = "black";
};

inline void from_json(const nlohmann::json& j, Frame& f)
{
    j.at("x").get_to(f.x);
    j.at("y").get_to(f.y);
    j.at("w").get_to(f.w);
    j.at("h").get_to(f.h);
    // j.at("borderRadius").get_to(f.borderRadius);
    j.at("thickness").get_to(f.thickness);
    j.at("color").get_to(f.color);
    // j.at("bgColor").get_to(f.bgColor);
}

inline void from_json(const nlohmann::json& j, Text& t)
{
    j.at("x").get_to(t.x);
    j.at("y").get_to(t.y);
    j.at("w").get_to(t.w);
    j.at("h").get_to(t.h);
    j.at("text").get_to(t.text);
    j.at("color").get_to(t.color);
}

} // namespace JX11::Gui::JsonData