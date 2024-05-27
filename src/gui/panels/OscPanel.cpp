#include "OscPanel.h"
#include "gui/lookandfeel/JX11LookAndFeel.h"
#include "gui/widgets/CustomSlider.h"
#include "processor/PluginProcessor.h"
#include <memory>

namespace JX11::Gui
{
OscPanel::OscPanel(Processor::JX11AudioProcessor& p, nlohmann::json& guiData)
    : IPanel("Oscillator Panel", guiData)
{
    auto init_slider = [](CustomSlider& slider) {
        slider.setNumDecimalPlacesToDisplay(0);
        slider.setCustomPaintLambda([&slider](juce::Graphics& g) {
            const auto text = slider.getTextFromValue(slider.getValue());
            const auto bounds = slider.getLocalBounds();
            g.setFont(getDefaultFont(18.f));
            g.drawText(text, bounds, juce::Justification::centred);
        });
    };

    oscMixSlider = std::make_unique<AttachedComponent<CustomSlider, juce::SliderParameterAttachment>>(
        *p.getParams().oscMixParam,
        *this, init_slider,
        "Osc Mix",
        juce::Slider::SliderStyle::RotaryVerticalDrag);
    oscTuneSlider = std::make_unique<AttachedComponent<CustomSlider, juce::SliderParameterAttachment>>(
        *p.getParams().oscTuneParam,
        *this, init_slider,
        "Osc Tune",
        juce::Slider::SliderStyle::RotaryVerticalDrag);
}

} // namespace JX11::Gui