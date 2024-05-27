#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace JX11::Gui
{

class CustomToggleButton : public juce::ToggleButton
{
public:
    explicit CustomToggleButton(const juce::String& name);

    juce::String getTooltip() override;

    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

    void setCustomTooltipLambda(std::function<juce::String()> inLambda);

private:
    std::function<juce::String()> mCustomTooltipLambda = nullptr;
};

} // namespace JX11::Gui
