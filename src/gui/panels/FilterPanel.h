#pragma once

#include "gui/AttachedComponent.hpp"
#include "gui/IPanel.h"
#include "gui/widgets/CustomSlider.h"
#include "processor/PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace JX11::Gui
{

class FilterPanel final : public IPanel
{
public:
    FilterPanel() = delete;
    explicit FilterPanel(Processor::JX11AudioProcessor& p, nlohmann::json& guiData);
    ~FilterPanel() final = default;

    //==============================================================================
private:
    AttachedComponent<CustomSlider, juce::SliderParameterAttachment> filterFreqSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterPanel)
};

} // namespace JX11::Gui