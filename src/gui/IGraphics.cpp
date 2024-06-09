#include "IGraphics.h"
#include "gui/IGraphics.h"
#include "gui/json/Graphics.h"
#include "lookandfeel/JX11LookAndFeel.h"

namespace JX11::Gui
{

void IGraphics::loadDataFromJson(const nlohmann::json& inJson)
{
    elements.clear();
    if (inJson.contains(JsonData::kFrameType)) {
        auto frames = inJson[JsonData::kFrameType].get<std::vector<JsonData::Frame>>();
        for (auto& frame : frames) {
            elements.emplace_back(std::make_unique<FrameElement>(frame));
        }
    }

    if (inJson.contains(JsonData::kTextType)) {
        auto texts = inJson[JsonData::kTextType].get<std::vector<JsonData::Text>>();
        for (auto& text : texts) {
            elements.emplace_back(std::make_unique<TextElement>(text));
        }
    }
}

void IGraphics::draw(juce::Graphics& g) const
{
    for (const auto& element : elements) {
        element->draw(g);
    }
}

void FrameElement::draw(juce::Graphics& g) const
{
    g.setColour(juce::Colours::findColourForName(frameData.color, juce::Colours::black));
    g.drawRect(frameData.x, frameData.y, frameData.w, frameData.h, frameData.thickness);
}

void TextElement::draw(juce::Graphics& g) const
{
    g.setColour(juce::Colours::findColourForName(textData.color, juce::Colours::black));
    g.setFont(getDefaultFont());
    g.drawText(textData.text, textData.x, textData.y, textData.w, textData.h, juce::Justification::centred);
}

} // namespace JX11::Gui