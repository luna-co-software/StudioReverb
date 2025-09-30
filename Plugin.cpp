/*
 * Studio Reverb Plugin Implementation
 */

#include "DistrhoPlugin.hpp"
#include "DSP.hpp"
#include <cstring>

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------

class StudioReverbPlugin : public Plugin
{
public:
    StudioReverbPlugin()
        : Plugin(paramCount, 16, 1),  // 16 programs, 1 state
          dsp(getSampleRate())
    {
        // Load default program
        loadProgram(0);
    }

protected:
    // -------------------------------------------------------------------
    // Information

    const char* getLabel() const override
    {
        return "StudioReverb";
    }

    const char* getDescription() const override
    {
        return "High-quality reverb with four distinct algorithms";
    }

    const char* getMaker() const override
    {
        return "Luna Co. Audio";
    }

    const char* getHomePage() const override
    {
        return "https://github.com/luna-co-software/StudioReverb";
    }

    const char* getLicense() const override
    {
        return "GPL-3.0";
    }

    uint32_t getVersion() const override
    {
        return d_version(1, 0, 0);
    }

    int64_t getUniqueId() const override
    {
        return d_cconst('S', 't', 'R', 'v');
    }

    // -------------------------------------------------------------------
    // Init

    void initParameter(uint32_t index, Parameter& parameter) override
    {
        switch (index)
        {
        case paramReverbType:
            parameter.hints = kParameterIsAutomable | kParameterIsInteger;
            parameter.name = "Type";
            parameter.symbol = "type";
            parameter.unit = "";
            parameter.ranges.def = REVERB_ROOM;
            parameter.ranges.min = 0;
            parameter.ranges.max = REVERB_TYPE_COUNT - 1;
            parameter.enumValues.count = REVERB_TYPE_COUNT;
            parameter.enumValues.restrictedMode = true;
            {
                ParameterEnumerationValue* values = new ParameterEnumerationValue[REVERB_TYPE_COUNT];
                values[0].label = "Room";
                values[0].value = REVERB_ROOM;
                values[1].label = "Hall";
                values[1].value = REVERB_HALL;
                values[2].label = "Plate";
                values[2].value = REVERB_PLATE;
                values[3].label = "Early Reflections";
                values[3].value = REVERB_EARLY_REFLECTIONS;
                parameter.enumValues.values = values;
            }
            break;

        case paramDry:
            parameter.hints = kParameterIsAutomable;
            parameter.name = "Dry Level";
            parameter.symbol = "dry";
            parameter.unit = "%";
            parameter.ranges.def = 100.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;

        case paramEarly:
            parameter.hints = kParameterIsAutomable;
            parameter.name = "Early Level";
            parameter.symbol = "early";
            parameter.unit = "%";
            parameter.ranges.def = 75.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;

        case paramLate:
            parameter.hints = kParameterIsAutomable;
            parameter.name = "Late Level";
            parameter.symbol = "late";
            parameter.unit = "%";
            parameter.ranges.def = 75.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;

        case paramSize:
            parameter.hints = kParameterIsAutomable;
            parameter.name = "Size";
            parameter.symbol = "size";
            parameter.unit = "%";
            parameter.ranges.def = 50.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;

        case paramWidth:
            parameter.hints = kParameterIsAutomable;
            parameter.name = "Width";
            parameter.symbol = "width";
            parameter.unit = "%";
            parameter.ranges.def = 100.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;

        case paramPredelay:
            parameter.hints = kParameterIsAutomable;
            parameter.name = "Pre-Delay";
            parameter.symbol = "predelay";
            parameter.unit = "ms";
            parameter.ranges.def = 10.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 200.0f;
            break;

        case paramDecay:
            parameter.hints = kParameterIsAutomable;
            parameter.name = "Decay";
            parameter.symbol = "decay";
            parameter.unit = "s";
            parameter.ranges.def = 2.0f;
            parameter.ranges.min = 0.1f;
            parameter.ranges.max = 10.0f;
            break;

        case paramDiffuse:
            parameter.hints = kParameterIsAutomable;
            parameter.name = "Diffusion";
            parameter.symbol = "diffuse";
            parameter.unit = "%";
            parameter.ranges.def = 70.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;

        case paramDamping:
            parameter.hints = kParameterIsAutomable;
            parameter.name = "Damping";
            parameter.symbol = "damping";
            parameter.unit = "%";
            parameter.ranges.def = 50.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;

        case paramModulation:
            parameter.hints = kParameterIsAutomable;
            parameter.name = "Modulation";
            parameter.symbol = "modulation";
            parameter.unit = "%";
            parameter.ranges.def = 20.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;

        case paramLowCut:
            parameter.hints = kParameterIsAutomable;
            parameter.name = "Low Cut";
            parameter.symbol = "lowcut";
            parameter.unit = "Hz";
            parameter.ranges.def = 20.0f;
            parameter.ranges.min = 20.0f;
            parameter.ranges.max = 500.0f;
            break;

        case paramHighCut:
            parameter.hints = kParameterIsAutomable;
            parameter.name = "High Cut";
            parameter.symbol = "highcut";
            parameter.unit = "Hz";
            parameter.ranges.def = 16000.0f;
            parameter.ranges.min = 1000.0f;
            parameter.ranges.max = 20000.0f;
            break;
        }
    }

    void initProgramName(uint32_t index, String& programName) override
    {
        switch (index)
        {
        case 0:
            programName = "Default";
            break;
        case 1:
            programName = "Small Room";
            break;
        case 2:
            programName = "Medium Room";
            break;
        case 3:
            programName = "Large Room";
            break;
        case 4:
            programName = "Small Hall";
            break;
        case 5:
            programName = "Concert Hall";
            break;
        case 6:
            programName = "Cathedral";
            break;
        case 7:
            programName = "Bright Plate";
            break;
        case 8:
            programName = "Dark Plate";
            break;
        case 9:
            programName = "Vintage Plate";
            break;
        case 10:
            programName = "Subtle";
            break;
        case 11:
            programName = "Ambient";
            break;
        case 12:
            programName = "Dense";
            break;
        case 13:
            programName = "Spacious";
            break;
        case 14:
            programName = "Short";
            break;
        case 15:
            programName = "Long";
            break;
        }
    }

    void initState(uint32_t index, String& stateKey, String& defaultStateValue) override
    {
        if (index == 0)
        {
            stateKey = "preset";
            defaultStateValue = "0";  // Default preset
        }
    }

    // -------------------------------------------------------------------
    // Internal data

    float getParameterValue(uint32_t index) const override
    {
        return dsp.getParameterValue(index);
    }

    void setParameterValue(uint32_t index, float value) override
    {
        dsp.setParameterValue(index, value);
    }

    void loadProgram(uint32_t index) override
    {
        switch (index)
        {
        case 0:  // Default
            setParameterValue(paramReverbType, REVERB_ROOM);
            setParameterValue(paramDry, 100.0f);
            setParameterValue(paramEarly, 75.0f);
            setParameterValue(paramLate, 75.0f);
            setParameterValue(paramSize, 50.0f);
            setParameterValue(paramWidth, 100.0f);
            setParameterValue(paramPredelay, 10.0f);
            setParameterValue(paramDecay, 2.0f);
            setParameterValue(paramDiffuse, 70.0f);
            setParameterValue(paramDamping, 50.0f);
            setParameterValue(paramModulation, 20.0f);
            setParameterValue(paramLowCut, 20.0f);
            setParameterValue(paramHighCut, 16000.0f);
            break;

        case 1:  // Small Room
            setParameterValue(paramReverbType, REVERB_ROOM);
            setParameterValue(paramDry, 85.0f);
            setParameterValue(paramEarly, 90.0f);
            setParameterValue(paramLate, 60.0f);
            setParameterValue(paramSize, 25.0f);
            setParameterValue(paramWidth, 80.0f);
            setParameterValue(paramPredelay, 5.0f);
            setParameterValue(paramDecay, 0.8f);
            setParameterValue(paramDiffuse, 60.0f);
            setParameterValue(paramDamping, 70.0f);
            break;

        case 2:  // Medium Room
            setParameterValue(paramReverbType, REVERB_ROOM);
            setParameterValue(paramDry, 80.0f);
            setParameterValue(paramEarly, 80.0f);
            setParameterValue(paramLate, 70.0f);
            setParameterValue(paramSize, 50.0f);
            setParameterValue(paramWidth, 90.0f);
            setParameterValue(paramPredelay, 10.0f);
            setParameterValue(paramDecay, 1.5f);
            setParameterValue(paramDiffuse, 70.0f);
            setParameterValue(paramDamping, 60.0f);
            break;

        case 3:  // Large Room
            setParameterValue(paramReverbType, REVERB_ROOM);
            setParameterValue(paramDry, 75.0f);
            setParameterValue(paramEarly, 70.0f);
            setParameterValue(paramLate, 80.0f);
            setParameterValue(paramSize, 75.0f);
            setParameterValue(paramWidth, 100.0f);
            setParameterValue(paramPredelay, 20.0f);
            setParameterValue(paramDecay, 2.5f);
            setParameterValue(paramDiffuse, 80.0f);
            setParameterValue(paramDamping, 50.0f);
            break;

        case 4:  // Small Hall
            setParameterValue(paramReverbType, REVERB_HALL);
            setParameterValue(paramDry, 70.0f);
            setParameterValue(paramEarly, 50.0f);
            setParameterValue(paramLate, 85.0f);
            setParameterValue(paramSize, 60.0f);
            setParameterValue(paramWidth, 100.0f);
            setParameterValue(paramPredelay, 25.0f);
            setParameterValue(paramDecay, 2.0f);
            setParameterValue(paramDiffuse, 75.0f);
            setParameterValue(paramDamping, 55.0f);
            setParameterValue(paramModulation, 15.0f);
            break;

        case 5:  // Concert Hall
            setParameterValue(paramReverbType, REVERB_HALL);
            setParameterValue(paramDry, 60.0f);
            setParameterValue(paramEarly, 40.0f);
            setParameterValue(paramLate, 90.0f);
            setParameterValue(paramSize, 85.0f);
            setParameterValue(paramWidth, 100.0f);
            setParameterValue(paramPredelay, 35.0f);
            setParameterValue(paramDecay, 3.5f);
            setParameterValue(paramDiffuse, 85.0f);
            setParameterValue(paramDamping, 45.0f);
            setParameterValue(paramModulation, 20.0f);
            break;

        case 6:  // Cathedral
            setParameterValue(paramReverbType, REVERB_HALL);
            setParameterValue(paramDry, 50.0f);
            setParameterValue(paramEarly, 30.0f);
            setParameterValue(paramLate, 95.0f);
            setParameterValue(paramSize, 100.0f);
            setParameterValue(paramWidth, 100.0f);
            setParameterValue(paramPredelay, 50.0f);
            setParameterValue(paramDecay, 6.0f);
            setParameterValue(paramDiffuse, 90.0f);
            setParameterValue(paramDamping, 35.0f);
            setParameterValue(paramModulation, 25.0f);
            break;

        case 7:  // Bright Plate
            setParameterValue(paramReverbType, REVERB_PLATE);
            setParameterValue(paramDry, 85.0f);
            setParameterValue(paramEarly, 80.0f);
            setParameterValue(paramLate, 75.0f);
            setParameterValue(paramPredelay, 0.0f);
            setParameterValue(paramDecay, 2.0f);
            setParameterValue(paramDiffuse, 85.0f);
            setParameterValue(paramDamping, 20.0f);
            setParameterValue(paramModulation, 30.0f);
            setParameterValue(paramHighCut, 18000.0f);
            break;

        case 8:  // Dark Plate
            setParameterValue(paramReverbType, REVERB_PLATE);
            setParameterValue(paramDry, 80.0f);
            setParameterValue(paramEarly, 75.0f);
            setParameterValue(paramLate, 80.0f);
            setParameterValue(paramPredelay, 5.0f);
            setParameterValue(paramDecay, 2.5f);
            setParameterValue(paramDiffuse, 80.0f);
            setParameterValue(paramDamping, 70.0f);
            setParameterValue(paramModulation, 20.0f);
            setParameterValue(paramHighCut, 8000.0f);
            break;

        case 9:  // Vintage Plate
            setParameterValue(paramReverbType, REVERB_PLATE);
            setParameterValue(paramDry, 75.0f);
            setParameterValue(paramEarly, 85.0f);
            setParameterValue(paramLate, 70.0f);
            setParameterValue(paramPredelay, 10.0f);
            setParameterValue(paramDecay, 3.0f);
            setParameterValue(paramDiffuse, 75.0f);
            setParameterValue(paramDamping, 50.0f);
            setParameterValue(paramModulation, 40.0f);
            setParameterValue(paramHighCut, 12000.0f);
            break;

        // Add more presets as needed...
        default:
            // Load default for unimplemented presets
            loadProgram(0);
            break;
        }
    }

    String getState(const char* key) const override
    {
        if (std::strcmp(key, "preset") == 0)
        {
            // Return current preset index as string
            return String(getCurrentProgram());
        }
        return String();
    }

    void setState(const char* key, const char* value) override
    {
        if (std::strcmp(key, "preset") == 0)
        {
            // Load preset from saved state
            int preset = std::atoi(value);
            if (preset >= 0 && preset < 16)
                loadProgram(preset);
        }
    }

    // -------------------------------------------------------------------
    // Process

    void activate() override
    {
        // Nothing to do here
    }

    void deactivate() override
    {
        // Mute reverb tails
        dsp.mute();
    }

    void run(const float** inputs, float** outputs, uint32_t frames) override
    {
        dsp.run(inputs, outputs, frames);
    }

    void sampleRateChanged(double newSampleRate) override
    {
        dsp.sampleRateChanged(newSampleRate);
    }

    // -------------------------------------------------------------------

private:
    StudioReverbDSP dsp;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StudioReverbPlugin)
};

// -----------------------------------------------------------------------------

Plugin* createPlugin()
{
    return new StudioReverbPlugin();
}

// -----------------------------------------------------------------------------

END_NAMESPACE_DISTRHO