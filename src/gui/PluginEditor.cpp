#include "PluginEditor.h"
#include "gui/PluginEditor.h"
#include "gui/panels/MainPanel.h"
#include "melatonin_perfetto/melatonin_perfetto.h"
#include "json/Graphics.h"
#include "json/Panels.h"

namespace JX11::Gui
{
//==============================================================================
JX11AudioProcessorEditor::JX11AudioProcessorEditor(Processor::JX11AudioProcessor& p)
    : juce::AudioProcessorEditor(&p),
      mainPanel(p, panelsJson),
      tooltipWindow(mainPanel.getTooltipPanel(), 50)
{
    setLookAndFeel(&mLookAndFeel);
    tooltipWindow.setLookAndFeel(&mLookAndFeel);

    addAndMakeVisible(mainPanel);

    setResizable(true, false);
    setSize(kWindowWidth, kWindowHeight);
    setResizeLimits(kWindowWidth, kWindowHeight,
                    static_cast<int>(kWindowWidth * kMaxResize), static_cast<int>(kWindowHeight * kMaxResize));
    getConstrainer()->setFixedAspectRatio(kWindowRatio);

    setWantsKeyboardFocus(true);

    // Load the components positions from the json file.
    loadPanels();

    // Load the graphics elements that will be painted to the screen.
    loadGraphics();
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

    graphics.draw(g);
}

void JX11AudioProcessorEditor::resized()
{
    TRACE_COMPONENT();

    mainPanel.setSize(kWindowWidth, kWindowHeight);
    mainPanel.setTransform(juce::AffineTransform::scale(static_cast<float>(getWidth()) / static_cast<float>(kWindowWidth)));
    mainPanel.resized();
}

std::optional<nlohmann::json> JX11AudioProcessorEditor::getJsonFromFile(const std::string& inFileName) const
{
    juce::File jsonFile(juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory)
                            .getChildFile(inFileName));

    if (!jsonFile.existsAsFile()) {
        jsonFile = juce::File(DATA_FOLDER).getChildFile(inFileName);
    }

    if (!jsonFile.existsAsFile()) {
        return std::nullopt;
    }

    try {
        return nlohmann::json::parse(jsonFile.loadFileAsString().toStdString());
    } catch (const std::exception& e) {
        DBG("getJsonFromFile " + inFileName + " -> " + e.what());
    }

    return std::nullopt;
}

void JX11AudioProcessorEditor::loadPanels()
{
    TRACE_COMPONENT();

    if (auto panels_json = getJsonFromFile(JsonData::kPanelsFileName); panels_json) {
        panelsJson = *panels_json;
        mainPanel.loadDataFromJson();
        mainPanel.recomputeBounds(true);
    }
}

void JX11AudioProcessorEditor::loadGraphics()
{
    TRACE_COMPONENT();

    if (auto graphicsJson = getJsonFromFile(JsonData::kGraphicsFileName); graphicsJson) {
        graphics.loadDataFromJson(*graphicsJson);
        repaint();
    }
}

bool JX11AudioProcessorEditor::keyPressed(const juce::KeyPress& key)
{
    if (key.getModifiers().isCtrlDown()) {
        if (key.isKeyCurrentlyDown('R')) {
            DBG("--- Reload GUI ---");
            loadPanels();
            loadGraphics();
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