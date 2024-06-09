#pragma once

#include "gui/json/Panels.h"
#include <juce_core/juce_core.h>
#include <optional>
#include <unordered_map>

namespace JX11::Gui
{

/**
 * @brief Interface for all panels.
 */
class IPanel : public juce::Component
{
public:
    IPanel(const std::string& componentName, nlohmann::json& guiData);
    ~IPanel() override = default;

    void addChild(juce::Component* panel);
    void resized() override;

    void loadDataFromJson();

    void recomputeBounds(bool recursive = true);

private:
    std::optional<juce::Rectangle<int>> getChildBounds(const std::string& childName);
    juce::Rectangle<int> getBoundsFromPosition(const JsonData::Position& position);

    nlohmann::json& guiJson;

    JsonData::ChildrenPositions positions;

    std::unordered_map<std::string, juce::Component*> children;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IPanel)
};

} // namespace JX11::Gui
