#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace JX11::Processor
{

//==============================================================================
class BaseProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    BaseProcessor();
    ~BaseProcessor() override = default;

    //==============================================================================
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

protected:
    std::atomic<bool> parametersChanged {false};

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseProcessor)
};

} // namespace JX11::Processor