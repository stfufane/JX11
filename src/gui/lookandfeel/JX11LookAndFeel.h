#pragma once

#include "BinaryData.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace JX11::Gui
{

inline juce::Font getDefaultFont(const float inPointHeight = 16.f)
{
    return juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::iosevkaregular_ttf,
                                                              BinaryData::iosevkaregular_ttfSize))
        .withPointHeight(inPointHeight);
}

class JX11LookAndFeel final : public juce::LookAndFeel_V4
{
public:
    explicit JX11LookAndFeel();
    ~JX11LookAndFeel() override = default;

    juce::Rectangle<int> getTooltipBounds(const juce::String& tipText,
                                          juce::Point<int> screenPos,
                                          juce::Rectangle<int> parentArea) override;

    void drawTooltip(juce::Graphics& g, const juce::String& text, int width, int height) override;

    void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          juce::Slider::SliderStyle, juce::Slider&) override;

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider&) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override;

    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive,
                           bool isHighlighted, bool isTicked,
                           bool hasSubMenu, const juce::String& text,
                           const juce::String& shortcutKeyText,
                           const juce::Drawable* icon, const juce::Colour* textColourToUse) override;

private:
    void setColors();
};

} // namespace JX11::Gui