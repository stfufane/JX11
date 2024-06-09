#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

namespace JX11::Gui::JsonData
{
inline static const std::string kPanelsFileName { "panels.json" };

// TODO add different position types for automatic layout (grid, flex)
enum class PositionType {
    eAbsolute, // Position in pixels
    eRelative, // Position in percentage
};

// Allow PositionType to be serialized to/from JSON
NLOHMANN_JSON_SERIALIZE_ENUM(PositionType,
                             {
                                 { PositionType::eAbsolute, "absolute" },
                                 { PositionType::eRelative, "relative" },
                             })

struct Position
{
    PositionType type = PositionType::eAbsolute;
    juce::Rectangle<int> coords;
};

using ChildrenPositions = std::unordered_map<std::string, Position>;

inline void from_json(const nlohmann::json& j, juce::Rectangle<int>& r)
{
    r = { j.at("x").get<int>(), j.at("y").get<int>(), j.at("w").get<int>(), j.at("h").get<int>() };
}

inline void from_json(const nlohmann::json& j, Position& p)
{
    j.at("type").get_to(p.type);
    from_json(j.at("coords"), p.coords);
}

} // namespace JX11::Gui::JsonData