#pragma once

#include "gui/AttachedComponent.hpp"
#include "gui/IPanel.h"
#include "gui/widgets/CustomSlider.h"
#include "processor/PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace JX11::Gui
{

class OscPanel final : public IPanel
{
public:
    OscPanel() = delete;
    explicit OscPanel(Processor::JX11AudioProcessor& p, nlohmann::json& guiData);
    ~OscPanel() final = default;

    //==============================================================================
private:
    std::unique_ptr<AttachedComponent<CustomSlider, juce::SliderParameterAttachment>> oscMixSlider;
    std::unique_ptr<AttachedComponent<CustomSlider, juce::SliderParameterAttachment>> oscTuneSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscPanel)
};

} // namespace JX11::Gui