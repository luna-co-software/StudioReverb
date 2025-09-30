/*
 * Studio Reverb DSP Header
 */

#ifndef STUDIO_REVERB_DSP_HPP_INCLUDED
#define STUDIO_REVERB_DSP_HPP_INCLUDED

#include "DistrhoPluginInfo.h"

// Freeverb3 includes
#include "freeverb/earlyref.hpp"
#include "freeverb/progenitor2.hpp"
#include "freeverb/nrevb.hpp"

// Buffer size for processing
static const uint32_t BUFFER_SIZE = 256;

class StudioReverbDSP
{
public:
    StudioReverbDSP(double sampleRate);
    ~StudioReverbDSP();

    // Parameter management
    float getParameterValue(uint32_t index) const;
    void setParameterValue(uint32_t index, float value);

    // Audio processing
    void run(const float** inputs, float** outputs, uint32_t frames);

    // Sample rate handling
    void sampleRateChanged(double sampleRate);

    // Mute all reverb tails
    void mute();

private:
    // Initialize reverb processors
    void initializeRoomReverb();
    void initializeHallReverb();
    void initializePlateReverb();
    void initializeEarlyReflections();

    // Process functions for each algorithm
    void processRoomReverb(const float** inputs, uint32_t frames, uint32_t offset);
    void processHallReverb(const float** inputs, uint32_t frames, uint32_t offset);
    void processPlateReverb(const float** inputs, uint32_t frames, uint32_t offset);
    void processEarlyReflections(const float** inputs, uint32_t frames, uint32_t offset);

    // Utility
    void muteAll();

    // State
    double sampleRate;
    float params[paramCount];
    ReverbType currentReverbType;

    // Mix levels
    float dryLevel;
    float earlyLevel;
    float lateLevel;

    // Room reverb processors
    fv3::earlyref_f roomEarly;
    fv3::progenitor2_f roomLate;

    // Hall reverb processors
    fv3::earlyref_f hallEarly;
    fv3::progenitor2_f hallLate;

    // Plate reverb processor
    fv3::nrevb_f plateReverb;

    // Early reflections only
    fv3::earlyref_f earlyOnly;

    // Processing buffers
    float early_out_buffer[2][BUFFER_SIZE];
    float late_out_buffer[2][BUFFER_SIZE];
};

#endif // STUDIO_REVERB_DSP_HPP_INCLUDED