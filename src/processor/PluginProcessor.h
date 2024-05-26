#pragma once

#include "BaseProcessor.h"
#include "Params.h"
#include "engine/Synth.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <melatonin_perfetto/melatonin_perfetto.h>

namespace JX11::Processor
{

//==============================================================================
class Jx11AudioProcessor final : public BaseProcessor, public juce::AudioProcessorParameter::Listener
{
public:
    Jx11AudioProcessor();
    ~Jx11AudioProcessor() final;
    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) final;
    void releaseResources() final;
    void reset() final;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) final;

    const Params& getParams() const noexcept { return mParams; }

    void parameterValueChanged(int parameterIndex, float newValue) final;
    void parameterGestureChanged(int, bool) final {}

private:
    void update();

    void splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2);
    void render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset);

    //==============================================================================
    Params mParams;

    Engine::Synth mSynth;

    //==============================================================================
#if PERFETTO
    std::unique_ptr<perfetto::TracingSession> tracingSession;
#endif

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Jx11AudioProcessor)
};

} // namespace JX11::Processor