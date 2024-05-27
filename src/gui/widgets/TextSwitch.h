#pragma once

#include "CustomToggleButton.h"
#include <juce_graphics/juce_graphics.h>

namespace JX11::Gui
{
class TextSwitch : public CustomToggleButton
{
public:
    TextSwitch() = delete;
    explicit TextSwitch(const juce::String& name, juce::String inOnText, juce::String inOffText, float inFontSize = 20.f);

    void paint(juce::Graphics& g) override;

    void setOnText(const juce::String& inText) { onText = inText; }
    void setOffText(const juce::String& inText) { offText = inText; }

private:
    juce::String onText;
    juce::String offText;
    float fontSize = 20.f;
};
} // namespace JX11::Gui