#pragma once

#include "gui/lookandfeel/JX11LookAndFeel.h"
#include "gui/panels/MainPanel.h"
#include "melatonin_inspector/melatonin_inspector.h"
#include "processor/PluginProcessor.h"
#include <nlohmann/json.hpp>

namespace JX11::Gui
{
class JX11AudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit JX11AudioProcessorEditor(Processor::JX11AudioProcessor& p);

    ~JX11AudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    bool keyPressed(const juce::KeyPress& key) final;

private:
    void loadPositions();

    nlohmann::json guiJson = nlohmann::json({}); // Init with an empty object instead of null.

    /**
     * @brief The main LookAndFeel that will be applied to all the children components
     */
    JX11LookAndFeel mLookAndFeel;

    MainPanel mainPanel;

    juce::TooltipWindow tooltipWindow;

    std::unique_ptr<melatonin::Inspector> inspector;

    static constexpr float kWindowRatio = 2.f;
    static constexpr int kWindowWidth = 840;
    static constexpr int kWindowHeight = 420;
    static constexpr float kMaxResize = 1.5f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JX11AudioProcessorEditor)
};
} // namespace JX11::Gui