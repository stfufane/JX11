#include "PluginProcessor.h"
#include "Utils.h"

namespace JX11::Processor
{

JX11AudioProcessor::JX11AudioProcessor()
    : mParams(*this)
{
#if PERFETTO
    MelatoninPerfetto::get().beginSession();
#endif
    for (auto& param : getParameters()) {
        param->addListener(this);
    }
}

JX11AudioProcessor::~JX11AudioProcessor()
{
    for (auto& param : getParameters()) {
        param->removeListener(this);
    }
#if PERFETTO
    MelatoninPerfetto::get().endSession();
#endif
}

//==============================================================================
void JX11AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    mSynth.allocateResources(sampleRate, samplesPerBlock);
    parametersChanged.store(true);
    reset();
}

void JX11AudioProcessor::releaseResources()
{
    mSynth.deallocateResources();
}

void JX11AudioProcessor::reset()
{
    mSynth.reset();
    mSynth.outputLevelSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(mParams.outputLevelParam->get()));
}

void JX11AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                      [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    TRACE_DSP();
    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear extra channel data.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    // Only recalculate when a parameter has changed.
    bool expected = true;
    if (isNonRealtime() || parametersChanged.compare_exchange_strong(expected, false)) {
        update();
    }

    splitBufferByEvents(buffer, midiMessages);

#ifdef JUCE_DEBUG
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        JX11::Utils::protectYourEars(buffer.getWritePointer(channel), buffer.getNumSamples());
    }
#endif
}

void JX11AudioProcessor::splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    TRACE_DSP();
    int bufferOffset = 0;

    // Loop through the MIDI messages, which are sorted by samplePosition,
    // the relative timestamp inside the current audio buffer.
    for (const auto metadata : midiMessages) {

        // Render the audio that happens before this event (if any).
        int samplesThisSegment = metadata.samplePosition - bufferOffset;
        if (samplesThisSegment > 0) {
            render(buffer, samplesThisSegment, bufferOffset);
            bufferOffset += samplesThisSegment;
        }

        // Handle the event. Ignore MIDI messages such as sysex.
        if (metadata.numBytes <= 3) {
            uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
            uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
            handleMIDI(metadata.data[0], data1, data2);
        }
    }

    // Render the audio after the last MIDI event. If there were no
    // MIDI events at all, this renders the entire buffer.
    int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
    if (samplesLastSegment > 0) {
        render(buffer, samplesLastSegment, bufferOffset);
    }

    midiMessages.clear();
}

void JX11AudioProcessor::parameterValueChanged([[maybe_unused]] int parameterIndex, [[maybe_unused]] float newValue)
{
    // This function is called when a parameter changes. We set a flag to
    // recalculate the synth parameters in the audio thread. This is done to
    // avoid doing the calculations in the GUI thread, which could cause
    // performance issues.
    parametersChanged.store(true);
}

void JX11AudioProcessor::handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2)
{
    TRACE_DSP();
    // Print out the MIDI message:
    // char s[16];
    // snprintf(s, 16, "%02hhX %02hhX %02hhX", data0, data1, data2);
    // DBG(s);

    // Control Change
    if ((data0 & 0xF0) == 0xB0) {
        if (data1 == 0x07) { // volume
            float volumeCtl = float(data2) / 127.0f;
            mParams.outputLevelParam->beginChangeGesture();
            mParams.outputLevelParam->setValueNotifyingHost(volumeCtl);
            mParams.outputLevelParam->endChangeGesture();
        }
    }

    // Program Change
    // if ((data0 & 0xF0) == 0xC0) {
    //     if (data1 < presets.size()) {
    //         setCurrentProgram(data1);
    //     }
    // }

    mSynth.midiMessage(data0, data1, data2);
}

void JX11AudioProcessor::render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset)
{
    TRACE_DSP();
    float* outputBuffers[2] = {nullptr, nullptr};
    outputBuffers[0] = buffer.getWritePointer(0) + bufferOffset;
    if (getTotalNumOutputChannels() > 1) {
        outputBuffers[1] = buffer.getWritePointer(1) + bufferOffset;
    }

    mSynth.render(outputBuffers, sampleCount);
}

void JX11AudioProcessor::update()
{
    TRACE_DSP();
    // This function is called from the audio callback whenever any of the
    // parameters have changed. Here, we simply recalculate everything when
    // this happens. This function is called at most once per audio block.
    // It could be optimized to recalculate only the things that have changed,
    // but doing the bookkeeping for that also has a cost. Still, it might be
    // worth it for parameters that are heavily automated.

    float sampleRate = float(getSampleRate());
    float inverseSampleRate = 1.0f / sampleRate;

    // The envelope is implemented using a simple one-pole filter, which creates
    // an analog-style exponential curve. The formulas below calculate the filter
    // coefficients for the attack, decay, and release stages.
    mSynth.envAttack = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * mParams.envAttackParam->get()));
    mSynth.envDecay = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * mParams.envDecayParam->get()));

    mSynth.envSustain = mParams.envSustainParam->get() / 100.0f;

    float envRelease = mParams.envReleaseParam->get();
    if (envRelease < 1.0f) {
        mSynth.envRelease = 0.75f; // extra fast release
    } else {
        mSynth.envRelease = std::exp(-inverseSampleRate * std::exp(5.5f - 0.075f * envRelease));
    }

    // How much noise to mix into the signal. This is a parabolic curve,
    // similar to creating a parameter with skew = 0.5.
    float noiseMix = mParams.noiseParam->get() / 100.0f;
    noiseMix *= noiseMix;
    mSynth.noiseMix = noiseMix * 0.06f;

    // How much to mix osc2 into the output. This is a value between 0 and 1.
    mSynth.oscMix = mParams.oscMixParam->get() / 100.0f;

    // Calculate the multiplication factor for detuning oscillator 2. This is
    // the same as 2^(N/12) where N is the number of (fractional) semitones.
    // This value will be multiplied with the oscillator period, which is why
    // detuning down is greater than 1, as lowering the pitch means the period
    // becomes longer. Vice versa for going up in pitch.
    float semi = mParams.oscTuneParam->get();
    float cent = mParams.oscFineParam->get();
    mSynth.detune = std::pow(1.059463094359f, -semi - 0.01f * cent);

    // Master tuning. See the book for a full explanation of what happens here.
    float octave = mParams.octaveParam->get(); // -2 to +2
    float tuning = mParams.tuningParam->get(); // -100 to +100
    float tuneInSemi = -36.3763f - 12.0f * octave - tuning / 100.0f;
    mSynth.tune = sampleRate * std::exp(0.05776226505f * tuneInSemi);

    // Mono or poly?
    mSynth.numVoices = (mParams.polyModeParam->getIndex() == 0) ? 1 : Engine::Synth::MAX_VOICES;

    // Convert decibels to gain. Use a smoother for this parameter.
    mSynth.outputLevelSmoother.setTargetValue(juce::Decibels::decibelsToGain(mParams.outputLevelParam->get()));

    // Filter velocity sensitivity, a value between -0.05 and +0.05.
    // If disabled, the velocity is completely ignored.
    float filterVelocity = mParams.filterVelocityParam->get();
    if (filterVelocity < -90.0f) {
        mSynth.velocitySensitivity = 0.0f; // turn off velocity
        mSynth.ignoreVelocity = true;
    } else {
        mSynth.velocitySensitivity = 0.0005f * filterVelocity;
        mSynth.ignoreVelocity = false;
    }

    // Use a lower update rate for the glide and filter envelope, 32 times
    // (= LFO_MAX) slower than the sample rate.
    const float inverseUpdateRate = inverseSampleRate * static_cast<float>(mSynth.LFO_MAX);

    // The LFO rate is an exponentional curve that maps the 0 - 1 parameter
    // value to 0.018 Hz - 20.09 Hz. Use this to calculate the phase increment
    // for a sine wave running at 1/32th the sample rate.
    float lfoRate = std::exp(7.0f * mParams.lfoRateParam->get() - 4.0f);
    mSynth.lfoInc = lfoRate * inverseUpdateRate * float(Engine::TWO_PI);

    // The vibrato parameter is a parabolic curve going from 0.0 for 0% up to
    // 0.05 for 100%. You can choose between PWM mode (to the left) and vibrato
    // mode (to the right). These values are used as the amplitude of the LFO
    // sine wave that modulates the oscillator periods.
    float vibrato = mParams.vibratoParam->get() / 200.0f;
    mSynth.vibrato = 0.2f * vibrato * vibrato;
    mSynth.pwmDepth = mSynth.vibrato;
    if (vibrato < 0.0f) {
        mSynth.vibrato = 0.0f;
    }

    // Need to glide?
    mSynth.glideMode = mParams.glideModeParam->getIndex();

    // Just like the envelope, glide is implemented using a one-pole filter
    // that is updated every 32 samples. Here we set the filter coefficient.
    // A smaller coefficient means the glide takes longer.
    float glideRate = mParams.glideRateParam->get();
    if (glideRate < 2.0f) {
        mSynth.glideRate = 1.0f; // no glide
    } else {
        mSynth.glideRate = 1.0f - std::exp(-inverseUpdateRate * std::exp(6.0f - 0.07f * glideRate));
    }

    // Glide bend goes from -36 semitones to +36 semitones.
    mSynth.glideBend = mParams.glideBendParam->get();

    // The filter's cutoff is set using the note's pitch and velocity. This
    // parameter shifts that cutoff up or down. Values are from -1.5 to 6.5.
    mSynth.filterKeyTracking = 0.08f * mParams.filterFreqParam->get() - 1.5f;

    // Filter Q. Starts at 1 and goes up to 20, approximately.
    float filterReso = mParams.filterResoParam->get() / 100.0f;
    mSynth.filterQ = std::exp(3.0f * filterReso);

    // Self-oscillation:
    // synth.filterQ = 1.0f / ((1.0f - filterReso + 1e-9) * (1.0f - filterReso + 1e-9));

    // When using both oscillators, and/or noise or large filter resonance,
    // the overall gain increases. This variable tries to compensate for that.
    // There is also a manual output level control, as the total volume also
    // depends on how many notes are playing, their envelopes, velocities, etc.
    mSynth.volumeTrim = 0.0008f * (3.2f - mSynth.oscMix - 25.0f * mSynth.noiseMix) * (1.5f - 0.5f * filterReso);

    // Filter LFO intensity. Parabolic curve from 0 to 2.5.
    float filterLFO = mParams.filterLFOParam->get() / 100.0f;
    mSynth.filterLFODepth = 2.5f * filterLFO * filterLFO;

    // The filter envelope uses the same formulas as the amplitude envelope
    // but runs 32 times slower, at the same update rate as the LFO.
    mSynth.filterAttack = std::exp(-inverseUpdateRate * std::exp(5.5f - 0.075f * mParams.filterAttackParam->get()));
    mSynth.filterDecay = std::exp(-inverseUpdateRate * std::exp(5.5f - 0.075f * mParams.filterDecayParam->get()));

    float filterSustain = mParams.filterSustainParam->get() / 100.0f;
    mSynth.filterSustain = filterSustain * filterSustain;

    mSynth.filterRelease = std::exp(-inverseUpdateRate * std::exp(5.5f - 0.075f * mParams.filterReleaseParam->get()));

    // Filter envelope intensity. Linear curve from -6.0 to +6.0.
    mSynth.filterEnvDepth = 0.06f * mParams.filterEnvParam->get();
}

} // namespace JX11::Processor

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JX11::Processor::JX11AudioProcessor();
}
