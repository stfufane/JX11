#pragma once

#include "gui/IPanel.h"
#include "gui/panels/FilterPanel.h"
#include "gui/panels/OscPanel.h"
#include "processor/PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace JX11::Gui
{
class MainPanel final : public IPanel
{
public:
    MainPanel() = delete;
    explicit MainPanel(Processor::JX11AudioProcessor& p, nlohmann::json& guiData);
    ~MainPanel() final = default;

    juce::Component* getTooltipPanel() { return &tooltipPanel; }

    //==============================================================================
private:
    OscPanel oscPanel;

    FilterPanel filterPanel;

    /**
     * @brief Container that will be put at the bottom to display all the tooltips at the same place.
     */
    juce::Component tooltipPanel { "tooltipPanel" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainPanel)
};

} // namespace JX11::Gui