#include "FilterPanel.h"
#include "gui/lookandfeel/JX11LookAndFeel.h"

namespace JX11::Gui
{
FilterPanel::FilterPanel(Processor::JX11AudioProcessor& p, nlohmann::json& guiData)
    : IPanel("Filter Panel", guiData),
      filterFreqSlider(
          *p.getParams().filterFreqParam, *this, [](CustomSlider& slider) {
              slider.setNumDecimalPlacesToDisplay(0);
              slider.setCustomPaintLambda([&slider](juce::Graphics& g) {
                  const auto text = slider.getTextFromValue(slider.getValue());
                  const auto bounds = slider.getLocalBounds();
                  g.setFont(getDefaultFont(18.f));
                  g.drawText(text, bounds, juce::Justification::centred);
              });
          },
          "Filter Freq", juce::Slider::SliderStyle::RotaryVerticalDrag)
{
}

} // namespace JX11::Gui