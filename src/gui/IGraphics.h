#pragma once

#include "gui/json/Graphics.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <nlohmann/json.hpp>

namespace JX11::Gui
{

class IGraphicElement
{
public:
    IGraphicElement() = default;
    virtual ~IGraphicElement() = default;

    //==============================================================================
    virtual void draw(juce::Graphics& g) const = 0;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IGraphicElement)
};

class FrameElement : public IGraphicElement
{
public:
    FrameElement() = delete;
    explicit FrameElement(const JsonData::Frame& inData) : frameData(inData) {}
    ~FrameElement() override = default;

    //==============================================================================
    void draw(juce::Graphics& g) const override;

private:
    JsonData::Frame frameData;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FrameElement)
};

class TextElement : public IGraphicElement
{
public:
    TextElement() = delete;
    explicit TextElement(const JsonData::Text& inData) : textData(inData) {}
    ~TextElement() override = default;

    //==============================================================================
    void draw(juce::Graphics& g) const override;

private:
    JsonData::Text textData;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextElement)
};

class IGraphics
{
public:
    IGraphics() = default;
    virtual ~IGraphics() = default;

    void loadDataFromJson(const nlohmann::json& inJson);

    //==============================================================================
    void draw(juce::Graphics& g) const;

private:
    std::vector<std::unique_ptr<IGraphicElement>> elements;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IGraphics)
};

} // namespace JX11::Gui