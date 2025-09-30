/*
 * Studio Reverb - Unified Dragonfly Reverb Plugin
 * Copyright (C) 2024 Luna Co. Audio
 * Based on Dragonfly Reverb by Michael Willis and Rob van den Berg
 */

#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_BRAND   "Luna Co. Audio"
#define DISTRHO_PLUGIN_NAME    "Studio Reverb"
#define DISTRHO_PLUGIN_URI     "urn:lunaco:studio-reverb"

#define DISTRHO_PLUGIN_HAS_UI        1
#define DISTRHO_PLUGIN_IS_RT_SAFE    1
#define DISTRHO_PLUGIN_NUM_INPUTS    2
#define DISTRHO_PLUGIN_NUM_OUTPUTS   2
#define DISTRHO_PLUGIN_WANT_PROGRAMS 1
#define DISTRHO_PLUGIN_WANT_STATE    1

#define DISTRHO_PLUGIN_LV2_CATEGORY "lv2:ReverbPlugin"
#define DISTRHO_PLUGIN_VST3_CATEGORIES "Fx|Reverb|Stereo"

#define DISTRHO_UI_USE_NANOVG 1
#define DISTRHO_UI_DEFAULT_WIDTH  700
#define DISTRHO_UI_DEFAULT_HEIGHT 500

// Parameter IDs
enum Parameters
{
    paramReverbType = 0,
    paramDry,
    paramEarly,
    paramLate,
    paramSize,
    paramWidth,
    paramPredelay,
    paramDecay,
    paramDiffuse,
    paramDamping,
    paramModulation,
    paramLowCut,
    paramHighCut,
    paramCount
};

// Reverb Types
enum ReverbType
{
    REVERB_ROOM = 0,
    REVERB_HALL,
    REVERB_PLATE,
    REVERB_EARLY_REFLECTIONS,
    REVERB_TYPE_COUNT
};

// Parameter visibility structure
struct ParameterVisibility
{
    bool showSize;
    bool showDecay;
    bool showDiffuse;
    bool showDamping;
    bool showModulation;
    bool showEarly;
    bool showLate;
};

#endif // DISTRHO_PLUGIN_INFO_H_INCLUDED