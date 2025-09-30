/*
 * Studio Reverb DSP Implementation
 * Combines all 4 Dragonfly reverb algorithms
 */

#include "DSP.hpp"
#include <cmath>
#include <cstring>
#include <algorithm>

StudioReverbDSP::StudioReverbDSP(double sampleRate)
    : sampleRate(sampleRate),
      currentReverbType(REVERB_ROOM)
{
    // Initialize parameters with defaults
    params[paramReverbType] = REVERB_ROOM;
    params[paramDry] = 100.0f;
    params[paramEarly] = 75.0f;
    params[paramLate] = 75.0f;
    params[paramSize] = 50.0f;
    params[paramWidth] = 100.0f;
    params[paramPredelay] = 10.0f;
    params[paramDecay] = 2.0f;
    params[paramDiffuse] = 70.0f;
    params[paramDamping] = 50.0f;
    params[paramModulation] = 20.0f;
    params[paramLowCut] = 20.0f;
    params[paramHighCut] = 16000.0f;

    // Initialize all reverb processors
    initializeRoomReverb();
    initializeHallReverb();
    initializePlateReverb();
    initializeEarlyReflections();

    // Apply initial parameters
    for (uint32_t i = 0; i < paramCount; i++) {
        setParameterValue(i, params[i]);
    }
}

StudioReverbDSP::~StudioReverbDSP()
{
}

void StudioReverbDSP::initializeRoomReverb()
{
    // Room reverb uses earlyref + progenitor2
    roomEarly.loadPresetReflection(FV3_EARLYREF_PRESET_1);
    roomEarly.setMuteOnChange(false);
    roomEarly.setdryr(0);  // No dry signal in processor
    roomEarly.setwet(0);   // 0dB wet signal
    roomEarly.setwidth(0.8f);
    roomEarly.setLRDelay(0.3f);
    roomEarly.setLRCrossApFreq(750, 4);
    roomEarly.setDiffusionApFreq(150, 4);
    roomEarly.setSampleRate(sampleRate);

    roomLate.setMuteOnChange(false);
    roomLate.setwet(0);   // 0dB wet signal
    roomLate.setdryr(0);  // No dry signal in processor
    roomLate.setwidth(1.0f);
    roomLate.setSampleRate(sampleRate);

    // Room-specific defaults
    roomLate.setRSFactor(1.0f);
    roomLate.setrt60(2.0f);
    roomLate.setidiffusion1(0.75f);
    roomLate.setodiffusion1(0.75f);
    roomLate.setdamp(8000.0f);
    roomLate.setoutputdamp(8000.0f);
}

void StudioReverbDSP::initializeHallReverb()
{
    // Hall reverb uses earlyref + progenitor2 with different settings
    hallEarly.loadPresetReflection(FV3_EARLYREF_PRESET_2);  // Different preset for hall
    hallEarly.setMuteOnChange(false);
    hallEarly.setdryr(0);
    hallEarly.setwet(0);
    hallEarly.setwidth(1.0f);
    hallEarly.setLRDelay(0.5f);
    hallEarly.setLRCrossApFreq(500, 4);
    hallEarly.setDiffusionApFreq(100, 4);
    hallEarly.setSampleRate(sampleRate);

    hallLate.setMuteOnChange(false);
    hallLate.setwet(0);
    hallLate.setdryr(0);
    hallLate.setwidth(1.0f);
    hallLate.setSampleRate(sampleRate);

    // Hall-specific defaults (larger space)
    hallLate.setRSFactor(2.5f);
    hallLate.setrt60(3.0f);
    hallLate.setidiffusion1(0.85f);
    hallLate.setodiffusion1(0.85f);
    hallLate.setdamp(6000.0f);
    hallLate.setoutputdamp(6000.0f);

    // Hall has modulation
    hallLate.setdccutfreq(100.0f);
    hallLate.setmoddepth(0.02f);
    hallLate.setmodfreq(0.5f);
}

void StudioReverbDSP::initializePlateReverb()
{
    // Plate reverb uses nrevb (plate simulation)
    plateReverb.setMuteOnChange(false);
    plateReverb.setdryr(0);
    plateReverb.setwet(0);
    plateReverb.setSampleRate(sampleRate);

    // Plate-specific defaults
    plateReverb.setrt60(2.5f);
    plateReverb.setdiffusion(0.8f);
    plateReverb.setbandwidthfreq(8000.0f);
    plateReverb.setdamping(0.0005f);

    // Plate modulation
    plateReverb.setmoddepth(0.03f);
    plateReverb.setmodfreq(1.0f);
}

void StudioReverbDSP::initializeEarlyReflections()
{
    // Early reflections only - no late reverb
    earlyOnly.loadPresetReflection(FV3_EARLYREF_PRESET_0);  // Simple early reflections
    earlyOnly.setMuteOnChange(false);
    earlyOnly.setdryr(0);
    earlyOnly.setwet(0);
    earlyOnly.setwidth(1.0f);
    earlyOnly.setLRDelay(0.2f);
    earlyOnly.setLRCrossApFreq(1000, 4);
    earlyOnly.setDiffusionApFreq(200, 4);
    earlyOnly.setSampleRate(sampleRate);
}

float StudioReverbDSP::getParameterValue(uint32_t index) const
{
    if (index < paramCount)
        return params[index];
    return 0.0f;
}

void StudioReverbDSP::setParameterValue(uint32_t index, float value)
{
    if (index >= paramCount)
        return;

    params[index] = value;

    switch(index) {
        case paramReverbType:
            currentReverbType = static_cast<ReverbType>(static_cast<int>(value + 0.5f));
            // Mute all processors when switching to avoid artifacts
            muteAll();
            break;

        case paramDry:
            dryLevel = value / 100.0f;
            break;

        case paramEarly:
            earlyLevel = value / 100.0f;
            break;

        case paramLate:
            lateLevel = value / 100.0f;
            break;

        case paramSize:
            {
                float sizeFactor = value / 50.0f;  // 0-100% -> 0-2x
                roomEarly.setRSFactor(sizeFactor);
                roomLate.setRSFactor(sizeFactor);
                hallEarly.setRSFactor(sizeFactor * 1.5f);  // Hall is larger
                hallLate.setRSFactor(sizeFactor * 1.5f);
                earlyOnly.setRSFactor(sizeFactor);
            }
            break;

        case paramWidth:
            {
                float width = value / 100.0f;
                roomEarly.setwidth(width);
                roomLate.setwidth(width);
                hallEarly.setwidth(width);
                hallLate.setwidth(width);
                plateReverb.setwidth(width);
                earlyOnly.setwidth(width);
            }
            break;

        case paramPredelay:
            roomEarly.setPreDelay(value);
            roomLate.setPreDelay(value);
            hallEarly.setPreDelay(value);
            hallLate.setPreDelay(value);
            plateReverb.setPreDelay(value);
            earlyOnly.setPreDelay(value);
            break;

        case paramDecay:
            roomLate.setrt60(value);
            hallLate.setrt60(value * 1.5f);  // Hall has longer decay
            plateReverb.setrt60(value);
            break;

        case paramDiffuse:
            {
                float diffusion = value / 100.0f;
                roomLate.setidiffusion1(diffusion);
                roomLate.setodiffusion1(diffusion);
                hallLate.setidiffusion1(diffusion);
                hallLate.setodiffusion1(diffusion);
                plateReverb.setdiffusion(diffusion);

                // Early reflections diffusion
                int diffuseStages = static_cast<int>(diffusion * 10);
                roomEarly.setDiffusionApFreq(150 + diffusion * 350, diffuseStages);
                hallEarly.setDiffusionApFreq(100 + diffusion * 400, diffuseStages);
                earlyOnly.setDiffusionApFreq(200 + diffusion * 300, diffuseStages);
            }
            break;

        case paramDamping:
            {
                float dampFreq = 20000.0f * (1.0f - value / 100.0f);
                roomLate.setdamp(dampFreq);
                roomLate.setoutputdamp(dampFreq);
                hallLate.setdamp(dampFreq);
                hallLate.setoutputdamp(dampFreq);
                plateReverb.setbandwidthfreq(dampFreq);
            }
            break;

        case paramModulation:
            {
                float modDepth = value / 100.0f * 0.05f;  // 0-5% pitch modulation
                float modFreq = 0.1f + value / 100.0f * 2.0f;  // 0.1-2.1 Hz

                hallLate.setmoddepth(modDepth);
                hallLate.setmodfreq(modFreq);
                plateReverb.setmoddepth(modDepth * 1.5f);  // Plate has more modulation
                plateReverb.setmodfreq(modFreq * 1.5f);
            }
            break;

        case paramLowCut:
            roomEarly.setoutputhpf(value);
            roomLate.setdccutfreq(value);
            hallEarly.setoutputhpf(value);
            hallLate.setdccutfreq(value);
            plateReverb.setdccutfreq(value);
            earlyOnly.setoutputhpf(value);
            break;

        case paramHighCut:
            roomEarly.setoutputlpf(value);
            hallEarly.setoutputlpf(value);
            earlyOnly.setoutputlpf(value);
            // Late reverb high cut is handled by damping
            break;
    }
}

void StudioReverbDSP::run(const float** inputs, float** outputs, uint32_t frames)
{
    // Process in blocks
    uint32_t offset = 0;

    while (offset < frames) {
        uint32_t buffer_frames = std::min(BUFFER_SIZE, frames - offset);

        // Clear output buffers
        std::memset(early_out_buffer[0], 0, buffer_frames * sizeof(float));
        std::memset(early_out_buffer[1], 0, buffer_frames * sizeof(float));
        std::memset(late_out_buffer[0], 0, buffer_frames * sizeof(float));
        std::memset(late_out_buffer[1], 0, buffer_frames * sizeof(float));

        // Process based on selected reverb type
        switch(currentReverbType) {
            case REVERB_ROOM:
                processRoomReverb(inputs, buffer_frames, offset);
                break;

            case REVERB_HALL:
                processHallReverb(inputs, buffer_frames, offset);
                break;

            case REVERB_PLATE:
                processPlateReverb(inputs, buffer_frames, offset);
                break;

            case REVERB_EARLY_REFLECTIONS:
                processEarlyReflections(inputs, buffer_frames, offset);
                break;
        }

        // Mix dry, early, and late signals
        for (uint32_t i = 0; i < buffer_frames; i++) {
            outputs[0][offset + i] = dryLevel * inputs[0][offset + i];
            outputs[1][offset + i] = dryLevel * inputs[1][offset + i];

            outputs[0][offset + i] += earlyLevel * early_out_buffer[0][i];
            outputs[1][offset + i] += earlyLevel * early_out_buffer[1][i];

            outputs[0][offset + i] += lateLevel * late_out_buffer[0][i];
            outputs[1][offset + i] += lateLevel * late_out_buffer[1][i];
        }

        offset += buffer_frames;
    }
}

void StudioReverbDSP::processRoomReverb(const float** inputs, uint32_t frames, uint32_t offset)
{
    // Process early reflections
    roomEarly.processreplace(
        const_cast<float*>(inputs[0]) + offset,
        const_cast<float*>(inputs[1]) + offset,
        early_out_buffer[0],
        early_out_buffer[1],
        frames);

    // Process late reverb
    roomLate.processreplace(
        const_cast<float*>(inputs[0]) + offset,
        const_cast<float*>(inputs[1]) + offset,
        late_out_buffer[0],
        late_out_buffer[1],
        frames);
}

void StudioReverbDSP::processHallReverb(const float** inputs, uint32_t frames, uint32_t offset)
{
    // Process early reflections
    hallEarly.processreplace(
        const_cast<float*>(inputs[0]) + offset,
        const_cast<float*>(inputs[1]) + offset,
        early_out_buffer[0],
        early_out_buffer[1],
        frames);

    // Process late reverb
    hallLate.processreplace(
        const_cast<float*>(inputs[0]) + offset,
        const_cast<float*>(inputs[1]) + offset,
        late_out_buffer[0],
        late_out_buffer[1],
        frames);

    // Hall combines early and late into single output (no separate early/late mix)
    // So we mix them here based on a fixed ratio
    for (uint32_t i = 0; i < frames; i++) {
        float mixedL = early_out_buffer[0][i] * 0.3f + late_out_buffer[0][i] * 0.7f;
        float mixedR = early_out_buffer[1][i] * 0.3f + late_out_buffer[1][i] * 0.7f;
        early_out_buffer[0][i] = mixedL;
        early_out_buffer[1][i] = mixedR;
        late_out_buffer[0][i] = 0;
        late_out_buffer[1][i] = 0;
    }
}

void StudioReverbDSP::processPlateReverb(const float** inputs, uint32_t frames, uint32_t offset)
{
    // Plate reverb processes everything as a single unit
    plateReverb.processreplace(
        const_cast<float*>(inputs[0]) + offset,
        const_cast<float*>(inputs[1]) + offset,
        early_out_buffer[0],
        early_out_buffer[1],
        frames);

    // Plate has no separate late reverb
    std::memset(late_out_buffer[0], 0, frames * sizeof(float));
    std::memset(late_out_buffer[1], 0, frames * sizeof(float));
}

void StudioReverbDSP::processEarlyReflections(const float** inputs, uint32_t frames, uint32_t offset)
{
    // Only early reflections, no late reverb
    earlyOnly.processreplace(
        const_cast<float*>(inputs[0]) + offset,
        const_cast<float*>(inputs[1]) + offset,
        early_out_buffer[0],
        early_out_buffer[1],
        frames);

    // No late reverb for early reflections mode
    std::memset(late_out_buffer[0], 0, frames * sizeof(float));
    std::memset(late_out_buffer[1], 0, frames * sizeof(float));
}

void StudioReverbDSP::sampleRateChanged(double newSampleRate)
{
    sampleRate = newSampleRate;

    // Update all processors
    roomEarly.setSampleRate(newSampleRate);
    roomLate.setSampleRate(newSampleRate);
    hallEarly.setSampleRate(newSampleRate);
    hallLate.setSampleRate(newSampleRate);
    plateReverb.setSampleRate(newSampleRate);
    earlyOnly.setSampleRate(newSampleRate);
}

void StudioReverbDSP::mute()
{
    muteAll();
}

void StudioReverbDSP::muteAll()
{
    roomEarly.mute();
    roomLate.mute();
    hallEarly.mute();
    hallLate.mute();
    plateReverb.mute();
    earlyOnly.mute();
}