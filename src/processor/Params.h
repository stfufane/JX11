#pragma once

#include "juce_core/system/juce_PlatformDefs.h"
#include <juce_audio_processors/juce_audio_processors.h>

namespace JX11::Processor
{

namespace ParamIds
{
#define PARAMETER_ID(str) const juce::ParameterID str(#str, 1);

PARAMETER_ID(oscMix)
PARAMETER_ID(oscTune)
PARAMETER_ID(oscFine)

PARAMETER_ID(glideMode)
PARAMETER_ID(glideRate)
PARAMETER_ID(glideBend)

PARAMETER_ID(filterFreq)
PARAMETER_ID(filterReso)
PARAMETER_ID(filterEnv)
PARAMETER_ID(filterLFO)
PARAMETER_ID(filterVelocity)
PARAMETER_ID(filterAttack)
PARAMETER_ID(filterDecay)
PARAMETER_ID(filterSustain)
PARAMETER_ID(filterRelease)

PARAMETER_ID(envAttack)
PARAMETER_ID(envDecay)
PARAMETER_ID(envSustain)
PARAMETER_ID(envRelease)

PARAMETER_ID(lfoRate)
PARAMETER_ID(vibrato)
PARAMETER_ID(noise)
PARAMETER_ID(octave)
PARAMETER_ID(tuning)
PARAMETER_ID(polyMode)
PARAMETER_ID(outputLevel)

#undef PARAMETER_ID
} // namespace ParamIds

struct Params
{
    Params() = delete;
    explicit Params(juce::AudioProcessor& processor)
    {

        auto oscMixStringFromValue = [](float value, int) -> juce::String {
            char s[16] = {0};
            snprintf(s, 16, "%4.0f:%2.0f", 100.0 - 0.5f * value, 0.5f * value);
            return juce::String(s);
        };

        oscMixParam = new juce::AudioParameterFloat(
            ParamIds::oscMix, "Osc Mix",
            juce::NormalisableRange<float>(0.0f, 100.0f), 0.0f,
            juce::AudioParameterFloatAttributes()
                .withLabel("%")
                .withStringFromValueFunction(oscMixStringFromValue));
        oscTuneParam = new juce::AudioParameterFloat(
            ParamIds::oscTune, "Osc Tune",
            juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f), -12.0f,
            juce::AudioParameterFloatAttributes().withLabel("semi"));
        oscFineParam = new juce::AudioParameterFloat(
            ParamIds::oscFine, "Osc Fine",
            juce::NormalisableRange<float>(-50.0f, 50.0f, 0.1f, 0.3f, true),
            0.0f, juce::AudioParameterFloatAttributes().withLabel("cent"));

        processor.addParameterGroup(
            std::make_unique<juce::AudioProcessorParameterGroup>(
                "osc", "Oscillator", "|",
                std::unique_ptr<juce::AudioParameterFloat>(oscMixParam),
                std::unique_ptr<juce::AudioParameterFloat>(oscTuneParam),
                std::unique_ptr<juce::AudioParameterFloat>(oscFineParam)));

        glideModeParam = new juce::AudioParameterChoice(
            ParamIds::glideMode, "Glide Mode",
            juce::StringArray {"Off", "Legato", "Always"}, 0);

        glideRateParam = new juce::AudioParameterFloat(
            ParamIds::glideRate, "Glide Rate",
            juce::NormalisableRange<float>(0.0f, 100.f, 1.0f), 35.0f,
            juce::AudioParameterFloatAttributes().withLabel("%"));

        glideBendParam = new juce::AudioParameterFloat(
            ParamIds::glideBend, "Glide Bend",
            juce::NormalisableRange<float>(-36.0f, 36.0f, 0.01f, 0.4f, true),
            0.0f, juce::AudioParameterFloatAttributes().withLabel("semi"));
        ;
        processor.addParameterGroup(
            std::make_unique<juce::AudioProcessorParameterGroup>(
                "glide", "Glide", "|",
                std::unique_ptr<juce::AudioParameterChoice>(glideModeParam),
                std::unique_ptr<juce::AudioParameterFloat>(glideRateParam),
                std::unique_ptr<juce::AudioParameterFloat>(glideBendParam)));

        filterFreqParam = new juce::AudioParameterFloat(
            ParamIds::filterFreq, "Filter Freq",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f,
            juce::AudioParameterFloatAttributes().withLabel("%"));

        filterResoParam = new juce::AudioParameterFloat(
            ParamIds::filterReso, "Filter Reso",
            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 15.0f,
            juce::AudioParameterFloatAttributes().withLabel("%"));

        filterEnvParam = new juce::AudioParameterFloat(
            ParamIds::filterEnv, "Filter Env",
            juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f), 50.0f,
            juce::AudioParameterFloatAttributes().withLabel("%"));

        filterLFOParam = new juce::AudioParameterFloat(
            ParamIds::filterLFO, "Filter LFO",
            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f,
            juce::AudioParameterFloatAttributes().withLabel("%"));

        auto filterVelocityStringFromValue = [](float value, int) {
            if (value < -90.0f)
                return juce::String("OFF");
            else
                return juce::String(value);
        };

        filterVelocityParam = new juce::AudioParameterFloat(
            ParamIds::filterVelocity, "Velocity",
            juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f), 0.0f,
            juce::AudioParameterFloatAttributes()
                .withLabel("%")
                .withStringFromValueFunction(filterVelocityStringFromValue));
        ;
        processor.addParameterGroup(
            std::make_unique<juce::AudioProcessorParameterGroup>(
                "filter", "Filter", "|",
                std::unique_ptr<juce::AudioParameterFloat>(filterFreqParam),
                std::unique_ptr<juce::AudioParameterFloat>(filterResoParam),
                std::unique_ptr<juce::AudioParameterFloat>(filterEnvParam),
                std::unique_ptr<juce::AudioParameterFloat>(filterLFOParam),
                std::unique_ptr<juce::AudioParameterFloat>(
                    filterVelocityParam)));

        filterAttackParam = new juce::AudioParameterFloat(
            ParamIds::filterAttack, "Filter Attack",
            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f,
            juce::AudioParameterFloatAttributes().withLabel("%"));

        filterDecayParam = new juce::AudioParameterFloat(
            ParamIds::filterDecay, "Filter Decay",
            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 30.0f,
            juce::AudioParameterFloatAttributes().withLabel("%"));

        filterSustainParam = new juce::AudioParameterFloat(
            ParamIds::filterSustain, "Filter Sustain",
            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f,
            juce::AudioParameterFloatAttributes().withLabel("%"));

        filterReleaseParam = new juce::AudioParameterFloat(
            ParamIds::filterRelease, "Filter Release",
            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 25.0f,
            juce::AudioParameterFloatAttributes().withLabel("%"));
        ;
        processor.addParameterGroup(
            std::make_unique<juce::AudioProcessorParameterGroup>(
                "filter_adsr", "Filter ADSR", "|",
                std::unique_ptr<juce::AudioParameterFloat>(filterAttackParam),
                std::unique_ptr<juce::AudioParameterFloat>(filterDecayParam),
                std::unique_ptr<juce::AudioParameterFloat>(filterSustainParam),
                std::unique_ptr<juce::AudioParameterFloat>(
                    filterReleaseParam)));

        envAttackParam = new juce::AudioParameterFloat(
            ParamIds::envAttack, "Env Attack",
            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f,
            juce::AudioParameterFloatAttributes().withLabel("%"));

        envDecayParam = new juce::AudioParameterFloat(
            ParamIds::envDecay, "Env Decay",
            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
            juce::AudioParameterFloatAttributes().withLabel("%"));

        envSustainParam = new juce::AudioParameterFloat(
            ParamIds::envSustain, "Env Sustain",
            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 100.0f,
            juce::AudioParameterFloatAttributes().withLabel("%"));

        envReleaseParam = new juce::AudioParameterFloat(
            ParamIds::envRelease, "Env Release",
            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 30.0f,
            juce::AudioParameterFloatAttributes().withLabel("%"));
        ;
        processor.addParameterGroup(
            std::make_unique<juce::AudioProcessorParameterGroup>(
                "env_adsr", "Env ADSR", "|",
                std::unique_ptr<juce::AudioParameterFloat>(envAttackParam),
                std::unique_ptr<juce::AudioParameterFloat>(envDecayParam),
                std::unique_ptr<juce::AudioParameterFloat>(envSustainParam),
                std::unique_ptr<juce::AudioParameterFloat>(envReleaseParam)));

        auto lfoRateStringFromValue = [](float value, int) {
            float lfoHz = std::exp(7.0f * value - 4.0f);
            return juce::String(lfoHz, 3);
        };

        lfoRateParam = new juce::AudioParameterFloat(
            ParamIds::lfoRate, "LFO Rate", juce::NormalisableRange<float>(),
            0.81f,
            juce::AudioParameterFloatAttributes()
                .withLabel("Hz")
                .withStringFromValueFunction(lfoRateStringFromValue));

        auto vibratoStringFromValue = [](float value, int) {
            if (value < 0.0f)
                return "PWM " + juce::String(-value, 1);
            else
                return juce::String(value, 1);
        };

        vibratoParam = new juce::AudioParameterFloat(
            ParamIds::vibrato, "Vibrato",
            juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f), 0.0f,
            juce::AudioParameterFloatAttributes()
                .withLabel("%")
                .withStringFromValueFunction(vibratoStringFromValue));

        noiseParam = new juce::AudioParameterFloat(
            ParamIds::noise, "Noise",
            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 0.0f,
            juce::AudioParameterFloatAttributes().withLabel("%"));

        octaveParam = new juce::AudioParameterFloat(
            ParamIds::octave, "Octave",
            juce::NormalisableRange<float>(-2.0f, 2.0f, 1.0f), 0.0f);

        tuningParam = new juce::AudioParameterFloat(
            ParamIds::tuning, "Tuning",
            juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f), 0.0f,
            juce::AudioParameterFloatAttributes().withLabel("cent"));

        polyModeParam = new juce::AudioParameterChoice(
            ParamIds::polyMode, "Polyphony", juce::StringArray {"Mono", "Poly"},
            1);

        outputLevelParam = new juce::AudioParameterFloat(
            ParamIds::outputLevel, "Output Level",
            juce::NormalisableRange<float>(-24.0f, 6.0f, 0.1f), 0.0f,
            juce::AudioParameterFloatAttributes().withLabel("dB"));
        ;
        processor.addParameterGroup(
            std::make_unique<juce::AudioProcessorParameterGroup>(
                "global_params", "Global Parameters", "|",
                std::unique_ptr<juce::AudioParameterFloat>(lfoRateParam),
                std::unique_ptr<juce::AudioParameterFloat>(vibratoParam),
                std::unique_ptr<juce::AudioParameterFloat>(noiseParam),
                std::unique_ptr<juce::AudioParameterFloat>(octaveParam),
                std::unique_ptr<juce::AudioParameterFloat>(tuningParam),
                std::unique_ptr<juce::AudioParameterChoice>(polyModeParam),
                std::unique_ptr<juce::AudioParameterFloat>(outputLevelParam)));
    }

    juce::AudioParameterFloat* oscMixParam;
    juce::AudioParameterFloat* oscTuneParam;
    juce::AudioParameterFloat* oscFineParam;

    juce::AudioParameterChoice* glideModeParam;
    juce::AudioParameterFloat* glideRateParam;
    juce::AudioParameterFloat* glideBendParam;

    juce::AudioParameterFloat* filterFreqParam;
    juce::AudioParameterFloat* filterResoParam;
    juce::AudioParameterFloat* filterEnvParam;
    juce::AudioParameterFloat* filterLFOParam;
    juce::AudioParameterFloat* filterVelocityParam;
    juce::AudioParameterFloat* filterAttackParam;
    juce::AudioParameterFloat* filterDecayParam;
    juce::AudioParameterFloat* filterSustainParam;
    juce::AudioParameterFloat* filterReleaseParam;

    juce::AudioParameterFloat* envAttackParam;
    juce::AudioParameterFloat* envDecayParam;
    juce::AudioParameterFloat* envSustainParam;
    juce::AudioParameterFloat* envReleaseParam;

    juce::AudioParameterFloat* lfoRateParam;
    juce::AudioParameterFloat* vibratoParam;
    juce::AudioParameterFloat* noiseParam;
    juce::AudioParameterFloat* octaveParam;
    juce::AudioParameterFloat* tuningParam;
    juce::AudioParameterFloat* outputLevelParam;
    juce::AudioParameterChoice* polyModeParam;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Params)
};

inline juce::String getParamID(juce::AudioProcessorParameter* param)
{
    if (auto paramWithID =
            dynamic_cast<juce::AudioProcessorParameterWithID*>(param))
        return paramWithID->paramID;

    return param->getName(50);
}

} // namespace JX11::Processor