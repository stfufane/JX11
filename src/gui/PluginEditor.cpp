#include "PluginEditor.h"
#include "JsonData.h"
#include "gui/panels/MainPanel.h"
#include "melatonin_perfetto/melatonin_perfetto.h"

namespace JX11::Gui
{
//==============================================================================
JX11AudioProcessorEditor::JX11AudioProcessorEditor(Processor::JX11AudioProcessor& p)
    : juce::AudioProcessorEditor(&p),
      mainPanel(p, guiJson),
      tooltipWindow(mainPanel.getTooltipPanel(), 50)
{
    setLookAndFeel(&mLookAndFeel);
    tooltipWindow.setLookAndFeel(&mLookAndFeel);

    addChildAndSetID(&mainPanel, "Main Panel");

    setResizable(true, false);
    setSize(kWindowWidth, kWindowHeight);
    setResizeLimits(kWindowWidth, kWindowHeight,
                    static_cast<int>(kWindowWidth * kMaxResize), static_cast<int>(kWindowHeight * kMaxResize));
    getConstrainer()->setFixedAspectRatio(kWindowRatio);

    setWantsKeyboardFocus(true);

    // Load the components positions from the json file.
    loadPositions();
}

JX11AudioProcessorEditor::~JX11AudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void JX11AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::whitesmoke);

    g.setColour(findColour(juce::Label::ColourIds::backgroundColourId));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(4.f), 4.f, 1.f);
}

void JX11AudioProcessorEditor::resized()
{
    TRACE_COMPONENT();

    mainPanel.setSize(kWindowWidth, kWindowHeight);
    mainPanel.setTransform(juce::AffineTransform::scale(static_cast<float>(getWidth()) / static_cast<float>(kWindowWidth)));
    mainPanel.resized();
}

void JX11AudioProcessorEditor::loadPositions()
{
    TRACE_COMPONENT();

    juce::File file(juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory)
                        .getChildFile(std::string(JsonData::kPositionsFileName)));

    if (!file.existsAsFile()) {
        file = juce::File(DATA_FOLDER).getChildFile(std::string(JsonData::kPositionsFileName));
    }

    if (file.existsAsFile()) {
        try {
            guiJson = nlohmann::json::parse(file.loadFileAsString().toStdString());
            mainPanel.loadDataFromJson();
            mainPanel.recomputeBounds(true);
        } catch (const std::exception& e) {
            DBG(e.what());
        }
    }
}

bool JX11AudioProcessorEditor::keyPressed(const juce::KeyPress& key)
{
    if (key.getModifiers().isCtrlDown()) {
        if (key.isKeyCurrentlyDown('R')) {
            loadPositions();
            return true;
        }

        if (key.isKeyCurrentlyDown('D')) {
            if (!inspector) {
                inspector = std::make_unique<melatonin::Inspector>(*this);
                inspector->onClose = [this]() { inspector.reset(); };
            }

            inspector->setVisible(true);
            return true;
        }
    }
    return false;
}

} // namespace JX11::Gui