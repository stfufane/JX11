#include "IPanel.h"
#include "gui/JsonData.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "melatonin_perfetto/melatonin_perfetto.h"

namespace JX11::Gui
{

IPanel::IPanel(const std::string& componentName, nlohmann::json& guiData)
    : juce::Component(componentName), guiJson(guiData)
{
    loadDataFromJson();
}

void IPanel::addChild(juce::Component* panel)
{
    addAndMakeVisible(panel);
    children[panel->getName().toStdString()] = panel;
}

void IPanel::recomputeBounds(bool recursive)
{
    for (const auto& [child_name, child_component] : children) {
        if (auto childBounds = getChildBounds(child_name); childBounds) {
            child_component->setBounds(*childBounds);
            if (recursive) {
                if (auto* panel = dynamic_cast<IPanel*>(child_component)) {
                    panel->recomputeBounds(true);
                }
            }
        } else {
            DBG("Could not find bounds for child " + child_name);
        }
    }
}

void IPanel::loadDataFromJson()
{
    if (auto component_name = getName().toStdString(); guiJson.contains(component_name)) {
        positions = guiJson[component_name].get<JsonData::ChildrenPositions>();
        for (auto& [child_name, child_component] : children) {
            if (auto* panel = dynamic_cast<IPanel*>(child_component)) {
                panel->loadDataFromJson();
            }
        }
    }
}

std::optional<juce::Rectangle<int>> IPanel::getChildBounds(const std::string& childName)
{
    auto child_position = positions.find(childName);
    if (child_position == positions.end()) {
        DBG("Could not find child data for " + childName);
        return {};
    }

    auto& position = child_position->second;
    return getBoundsFromPosition(position);
}

juce::Rectangle<int> IPanel::getBoundsFromPosition(const JsonData::Position& position)
{
    if (position.type == JsonData::PositionType::eAbsolute) {
        return position.coords;
    }

    return juce::Rectangle<int> { getBounds().getWidth() * position.coords.getX() / 100,
                                  getBounds().getHeight() * position.coords.getY() / 100,
                                  getBounds().getWidth() * position.coords.getWidth() / 100,
                                  getBounds().getHeight() * position.coords.getHeight() / 100 };
}

void IPanel::resized()
{
    TRACE_COMPONENT();
    recomputeBounds(false);
}

} // namespace JX11::Gui